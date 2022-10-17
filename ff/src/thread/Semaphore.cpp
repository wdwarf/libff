/*
 * Semaphore.cpp
 *
 *  Created on: 2016-10-26
 *      Author: liyawu
 */

#include <ff/Semaphore.h>
#include <stdarg.h>

#include <iostream>

#ifndef WIN32
#include <fcntl.h>
#endif

using namespace std;

NS_FF_BEG

Semaphore::Semaphore(int value) : m_named(false) {
#ifdef _WIN32
  this->m_previousCount = 0;
  this->m_sem = ::CreateSemaphore(NULL, 0, value, NULL);
#else
  this->m_sem = new sem_t, sem_init(this->m_sem, 0, value);
#endif
}

Semaphore::Semaphore(const string& name, int value) : m_named(true) {
#ifdef _WIN32
  this->m_previousCount = 0;
  this->m_sem = ::CreateSemaphore(NULL, 0, value, name.c_str());
#else
  this->m_sem = sem_open(name.c_str(), O_CREAT, 0644, value);
  if (nullptr == this->m_sem) {
    cerr << "failed to open sem" << endl;
  }
  this->m_name = name;
#endif
}

Semaphore::~Semaphore() {
#ifdef _WIN32
  ::CloseHandle(this->m_sem);
#else
  if (this->m_named) {
    sem_close(this->m_sem);
  } else {
    sem_destroy(this->m_sem);
    delete this->m_sem;
  }
#endif
}

bool Semaphore::wait(int mSec) {
#ifdef _WIN32
  return (WAIT_OBJECT_0 == WaitForSingleObject(this->m_sem, mSec));
#else
  if (mSec < 0) {
    return (0 == sem_wait(this->m_sem));
  }

#if 1
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  ts.tv_sec += mSec / 1000;
  ts.tv_nsec += (mSec % 1000) * 1000000;
  if (ts.tv_nsec >= 1000000000) {
    ts.tv_sec += 1;
    ts.tv_nsec -= 1000000000;
  }

  if (0 == sem_clockwait(this->m_sem, CLOCK_MONOTONIC, &ts)) return true;
#else
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += mSec / 1000;
  ts.tv_nsec += (mSec % 1000) * 1000000;
  if (ts.tv_nsec >= 1000000000) {
    ts.tv_sec += 1;
    ts.tv_nsec -= 1000000000;
  }

  if (0 == sem_timedwait(this->m_sem, &ts)) return true;
#endif

  // ETIMEDOUT == errno
  return false;
#endif
}

void Semaphore::release() {
#ifdef _WIN32
  (void)::ReleaseSemaphore(this->m_sem, 1, &this->m_previousCount);
#else
  (void)sem_post(this->m_sem);
#endif
}

int Semaphore::getValue() {
#ifdef _WIN32
  return this->m_previousCount;
#else
  int re = 0;
  sem_getvalue(this->m_sem, &re);
  return re;
#endif
}

void Semaphore::unlink() {
#ifdef _WIN32
#else
  if (this->m_named) {
    sem_unlink(this->m_name.c_str());
  }
#endif
}

NS_FF_END
