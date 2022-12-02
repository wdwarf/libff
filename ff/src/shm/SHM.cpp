/**
 * @file SHM.cpp
 * @auth DuckyLi
 * @date 2022-10-16 16:08:28
 * @description
 */
#include <ff/File.h>
#include <ff/SHM.h>

#include <cmath>
#include <iostream>

using namespace std;

NS_FF_BEG

SHM::SHM()
    : m_data(nullptr)
#ifdef WIN32
      ,
      m_hShm(NULL)
#else
      ,
      m_key(-1),
      m_shmId(-1)
#endif
{
}

SHM::~SHM() {}

void *SHM::create(const std::string &name, uint32_t size) {
#ifdef WIN32
  this->m_hShm =
      CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                         PAGE_READWRITE | SEC_COMMIT, 0, size, name.c_str());

  if (NULL == m_hShm) return nullptr;

  this->m_data = MapViewOfFile(m_hShm, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
#else
  File shmDir("/tmp/ff_shm");
  shmDir.mkdirs();
  File f(shmDir, name);
  if (!f.isExists()) {
    (void)system(("touch " + f.getPath()).c_str());
  }

  this->m_key = ftok(f.getPath().c_str(), 1);
  this->m_shmId = shmget(this->m_key, size, IPC_CREAT | 0666);
  this->m_data = shmat(this->m_shmId, 0, 0);
#endif

  return this->m_data;
}

void *SHM::data() { return this->m_data; }

void SHM::destroy() {
#ifdef WIN32
  if (NULL != this->m_hShm) {
    UnmapViewOfFile(this->m_data);
    CloseHandle(this->m_hShm);
  }

  this->m_hShm = NULL;
#else
  shmdt(this->m_data);
  shmctl(this->m_shmId, IPC_RMID, 0);
  this->m_shmId = -1;
  this->m_key = -1;
#endif

  this->m_data = nullptr;
}

SHMConnection::SHMConnection() {}

SHMConnection::~SHMConnection() {
  // this->m_semMasterIn->unlink();
  // this->m_semMasterOut->unlink();
  // this->m_semSlaveIn->unlink();
  // this->m_semSlaveOut->unlink();
}

bool SHMConnection::open(const std::string &name, uint32_t bufSize,
                         bool isMaster) {
  this->m_isMaster = isMaster;
  this->m_bufSize = bufSize;
  if (nullptr == this->m_shm.create(name, (m_bufSize + 4) * 2)) return false;

  this->m_semMasterIn =
      make_shared<Semaphore>("sem_" + name + "_master_in", 0, 1);
  this->m_semMasterOut =
      make_shared<Semaphore>("sem_" + name + "_master_out", 1, 1);
  this->m_semSlaveIn =
      make_shared<Semaphore>("sem_" + name + "_slave_in", 0, 1);
  this->m_semSlaveOut =
      make_shared<Semaphore>("sem_" + name + "_slave_out", 1, 1);

  return true;
}

int SHMConnection::write(const void *buf, int len) {
  uint8_t *data = (uint8_t *)m_shm.data();
  SemaphorePtr semIn = m_isMaster ? m_semMasterIn : m_semSlaveIn;
  SemaphorePtr semOut = m_isMaster ? m_semMasterOut : m_semSlaveOut;
  data = m_isMaster ? data : (data + (m_bufSize + 4));

  semOut->wait();
  uint32_t l = min((uint32_t)len, m_bufSize);
  memcpy(data, &l, 4);
  memcpy(data + 4, buf, l);
  semIn->release();
  return (len - l);
}

int SHMConnection::read(void *buf, int len, int msTimeout) {
  uint8_t *data = (uint8_t *)m_shm.data();
  SemaphorePtr semIn = m_isMaster ? m_semSlaveIn : m_semMasterIn;
  SemaphorePtr semOut = m_isMaster ? m_semSlaveOut : m_semMasterOut;
  data = m_isMaster ? (data + (m_bufSize + 4)) : data;

  if (!semIn->wait(msTimeout)) return -1;
  int l = 0;
  memcpy(&l, data, 4);
  memcpy(buf, data + 4, l);
  semOut->release();
  return l;
}

NS_FF_END
