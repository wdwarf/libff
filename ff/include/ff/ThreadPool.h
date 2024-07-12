/*
 * ThreadPool.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_THREADPOOL_H_
#define FF_THREADPOOL_H_

#include <ff/Noncopyable.h>
#include <ff/Thread.h>
#include <ff/ff_config.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <set>

NS_FF_BEG

class TaskThread;
class LIBFF_API ThreadPool : public Noncopyable {
 public:
  ThreadPool(unsigned int maxSize);
  virtual ~ThreadPool();

  void exec(RunnablePtr runnable);
  bool exec(RunnablePtr runnable, int32_t timeoutMs);

  template <class Func>
  void exec(Func func) {
    this->exec(MakeRunnable(func));
  }

  template <class Func>
  bool exec(Func func, int32_t timeoutMs) {
    return this->exec(MakeRunnable(func), timeoutMs);
  }

  unsigned int getIdelThreadCount() const;
  unsigned int getActiveThreadCount() const;
  void waitAll();

 private:
  friend class TaskThread;

  TaskThread* getThread(int32_t timeoutMs = -1);

  void PutTaskThreadPtr(TaskThread* p);

 private:
  mutable std::mutex m_mutex;
  std::condition_variable m_cond;
  unsigned int m_maxSize;
  std::set<TaskThread*> m_idelThreads;
  std::set<TaskThread*> m_busyThreads;
};

typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;

class LIBFF_API SimpleThreadPool {
 public:
  using TaskFunc = std::function<void()>;

  SimpleThreadPool();
  ~SimpleThreadPool();

  bool start(uint32_t threadCnt = 0);
  bool stop();
  void exec(const TaskFunc& func);

 private:
  class Impl;
  Impl* m_impl;
};

typedef std::shared_ptr<SimpleThreadPool> SimpleThreadPoolPtr;

NS_FF_END

#endif /* FF_THREADPOOL_H_ */
