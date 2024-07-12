/*
 * ThreadPool.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/BlockingList.h>
#include <ff/ThreadPool.h>

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

using namespace std;
using namespace std::placeholders;

NS_FF_BEG

class TaskThread : public Thread {
 public:
  TaskThread(ThreadPool* threadPool);
  ~TaskThread();

  void setTask(RunnablePtr task);
  void stop();

 private:
  void run() override;

 private:
  std::mutex m_mutex;
  std::condition_variable m_cond;
  RunnablePtr m_task;
  ThreadPool* m_threadPool;
  std::atomic_bool m_exit;
};

TaskThread::TaskThread(ThreadPool* threadPool)
    : m_threadPool(threadPool), m_exit(false) {}

TaskThread::~TaskThread() { this->join(); }

void TaskThread::setTask(RunnablePtr task) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_task = task;
  this->m_cond.notify_one();
}

void TaskThread::stop() {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_exit = true;
  this->m_cond.notify_one();
}

void TaskThread::run() {
  while (!this->m_exit) {
    unique_lock<mutex> lk(this->m_mutex);
    if (!this->m_task) {
      this->m_cond.wait(lk);
    }

    if (!this->m_task) continue;

    try {
      this->m_task->run();
      this->m_task.reset();
      this->m_threadPool->PutTaskThreadPtr(this);
    } catch (std::exception& e) {
      throw;
    }
  }
}

ThreadPool::ThreadPool(unsigned int maxSzie) : m_maxSize(maxSzie) {}

ThreadPool::~ThreadPool() {
  while (true) {
    {
      lock_guard<mutex> lk(this->m_mutex);
      if (this->m_busyThreads.empty()) break;
    }

    Thread::Sleep(100);
  }

  lock_guard<mutex> lk(this->m_mutex);
  for (auto& pThread : this->m_idelThreads) {
    pThread->stop();
  }

  for (auto& pThread : this->m_idelThreads) {
    delete pThread;
  }
}

void ThreadPool::exec(RunnablePtr runnable) {
  auto threadPtr = this->getThread();
  threadPtr->setTask(runnable);
}

bool ThreadPool::exec(RunnablePtr runnable, int32_t timeoutMs) {
  auto threadPtr = this->getThread(timeoutMs);
  if (!threadPtr) return false;
  threadPtr->setTask(runnable);
  return true;
}

unsigned int ThreadPool::getIdelThreadCount() const {
  lock_guard<mutex> lk(this->m_mutex);
  return this->m_idelThreads.size();
}

unsigned int ThreadPool::getActiveThreadCount() const {
  lock_guard<mutex> lk(this->m_mutex);
  return this->m_idelThreads.size() + this->m_busyThreads.size();
}

void ThreadPool::PutTaskThreadPtr(TaskThread* p) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_busyThreads.erase(p);
  this->m_idelThreads.insert(p);
  this->m_cond.notify_one();
}

TaskThread* ThreadPool::getThread(int32_t timeoutMs) {
  unique_lock<mutex> lk(this->m_mutex);

  if (!this->m_idelThreads.empty()) {
    auto it = this->m_idelThreads.begin();
    auto pThread = *it;
    this->m_idelThreads.erase(it);
    this->m_busyThreads.insert(pThread);
    return pThread;
  }

  if (this->m_idelThreads.size() + this->m_busyThreads.size() <
      this->m_maxSize) {
    auto pThread = new TaskThread(this);
    pThread->start();
    this->m_busyThreads.insert(pThread);
    return pThread;
  }

  if (timeoutMs >= 0) {
    if (!this->m_cond.wait_for(lk, std::chrono::milliseconds(timeoutMs),
                               [&] { return !this->m_idelThreads.empty(); }))
      return nullptr;
  } else
    this->m_cond.wait(lk, [&] { return !this->m_idelThreads.empty(); });

  auto it = this->m_idelThreads.begin();
  auto pThread = *it;
  this->m_idelThreads.erase(it);
  this->m_busyThreads.insert(pThread);
  return pThread;
}

void ThreadPool::waitAll() {
  unique_lock<mutex> lk(this->m_mutex);
  this->m_cond.wait(lk, [&] { return this->m_busyThreads.empty(); });
}

//////////////////////////////////////////

static DWORD DefaultThreadNum() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return (info.dwNumberOfProcessors + 2);
}

class SimpleThreadPool::Impl {
 public:
  Impl();
  ~Impl();

  bool start(uint32_t threadCnt = 0);
  bool stop();
  void exec(const SimpleThreadPool::TaskFunc& func);

 private:
  std::atomic_bool m_stoped;
  ff::BlockingList<SimpleThreadPool::TaskFunc> m_tasks;
  std::vector<std::thread> m_threads;
  void run();
};

SimpleThreadPool::Impl::Impl() { m_stoped = true; }

SimpleThreadPool::Impl::~Impl() { this->stop(); }

bool SimpleThreadPool::Impl::start(uint32_t threadCnt) {
  bool b = true;
  if (!m_stoped.compare_exchange_strong(b, false)) return false;

  if (threadCnt <= 0) threadCnt = DefaultThreadNum();

  for (uint32_t i = 0; i < threadCnt; ++i) {
    m_threads.push_back(std::thread([this] {
      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
      this->run();
    }));
  }

  return true;
}

bool SimpleThreadPool::Impl::stop() {
  bool b = false;
  if (!m_stoped.compare_exchange_strong(b, true)) return false;

  for (auto& t : m_threads) {
    if (t.joinable()) t.join();
  }

  m_tasks.clear();
  return true;
}

void SimpleThreadPool::Impl::exec(const SimpleThreadPool::TaskFunc& func) {
  if (m_stoped) return;
  m_tasks.push_back(func);
}

void SimpleThreadPool::Impl::run() {
  while (!m_stoped) {
    TaskFunc task;
    m_tasks.pop_front(task, 100);
    if (nullptr == task) continue;
    task();
  }
}

SimpleThreadPool::SimpleThreadPool() : m_impl(new Impl) {}

SimpleThreadPool::~SimpleThreadPool() { delete m_impl; }

bool SimpleThreadPool::start(uint32_t threadCnt) {
  return m_impl->start(threadCnt);
}

bool SimpleThreadPool::stop() { return m_impl->stop(); }

void SimpleThreadPool::exec(const SimpleThreadPool::TaskFunc& func) {
  m_impl->exec(func);
}

NS_FF_END
