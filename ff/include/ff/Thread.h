/*
 * Thread.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_THREAD_H_
#define FF_THREAD_H_

#include <ff/Exception.h>
#include <ff/Noncopyable.h>
#include <ff/Runnable.h>
#include <ff/ff_config.h>

#include <functional>
#include <memory>
#include <thread>

NS_FF_BEG

EXCEPTION_DEF2(ThreadException, RunnableException);

using ThreadIdT = uint32_t;

class LIBFF_API Thread : public Runnable, public Noncopyable {
 public:
  Thread();
  Thread(RunnablePtr runnable);
  Thread(RunnableFunc func);
  virtual ~Thread();

  void start();
  void join();
  bool isJoinable() const;
  void detach();
  ThreadIdT id();

  static void Sleep(unsigned int ms);
  static void(Yield)();
  static ThreadIdT CurrentThreadId();

 private:
  void run() override;

 private:
  RunnablePtr m_runnable;
  std::thread m_thread;
};

typedef std::shared_ptr<Thread> ThreadPtr;

NS_FF_END

#endif /* FF_THREAD_H_ */
