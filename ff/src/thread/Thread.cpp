/*
 * Thread.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/Thread.h>

#include <iostream>

using namespace std;

NS_FF_BEG

Thread::Thread() : m_runnable(shared_ptr<Thread>(this, [](void*) {})) {}

Thread::Thread(RunnablePtr runnable) { this->m_runnable = runnable; }

Thread::Thread(RunnableFunc func) : Thread(MakeRunnable(func)) {}

Thread::~Thread() { this->join(); }

void Thread::run() {}

ThreadIdT Thread::id() {
  if (!this->m_thread.joinable()) return 0;
  auto nativeHandle = this->m_thread.native_handle();
#ifdef _WIN32
  return ::GetThreadId(nativeHandle);
#else
  union {
    pthread_t pid;
    ThreadIdT id;
  } u;
  memset(&u, 0, sizeof(u));
  u.pid = nativeHandle;
  return u.id;
#endif
}

ThreadIdT Thread::CurrentThreadId() {
#ifdef _WIN32
  return ::GetCurrentThreadId();
#else
  union {
    pthread_t pid;
    ThreadIdT id;
  } u;
  memset(&u, 0, sizeof(u));
  u.pid = pthread_self();
  return u.id;
#endif
}

void Thread::start() {
  this->m_thread = thread([this] {
    try {
      this->m_runnable->run();
    } catch (std::exception& e) {
      throw;
    }
  });
}

void Thread::join() {
  if (this->isJoinable()) this->m_thread.join();
}

bool Thread::isJoinable() const { return this->m_thread.joinable(); }

void Thread::detach() { this->m_thread.detach(); }

void Thread::Sleep(unsigned int ms) {
  this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void(Thread::Yield)() { this_thread::yield(); }

NS_FF_END
