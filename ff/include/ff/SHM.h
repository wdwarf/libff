/**
 * @file SHM.h
 * @auth DuckyLi
 * @date 2022-10-16 16:02:22
 * @description
 */

#ifndef _FF_SHM_H_
#define _FF_SHM_H_

#include <ff/ff_config.h>

#include <string>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/shm.h>
#endif

#include <ff/Semaphore.h>

#include <memory>

NS_FF_BEG

class LIBFF_API SHM {
 public:
  SHM();
  ~SHM();

  void* create(const std::string& name, uint32_t size);
  void destroy();
  void* data();

 private:
  void* m_data;
#ifdef WIN32
  HANDLE m_hShm;
#else
  key_t m_key;
  int m_shmId;
#endif
};

class LIBFF_API SHMConnection {
 public:
  SHMConnection();
  ~SHMConnection();

  bool open(const std::string& name, uint32_t bufSize, bool isMaster);

  int write(const void* buf, int len);
  int read(void* buf, int len, int msTimeout = -1);

 private:
  bool m_isMaster;
  uint32_t m_bufSize;
  SHM m_shm;
  using SemaphorePtr = std::shared_ptr<Semaphore>;
  SemaphorePtr m_semMasterIn;
  SemaphorePtr m_semMasterOut;
  SemaphorePtr m_semSlaveIn;
  SemaphorePtr m_semSlaveOut;
};

NS_FF_END

#endif /** _FF_SHM_H_ */
