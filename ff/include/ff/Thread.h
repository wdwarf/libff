/*
 * Thread.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_THREAD_H_
#define FF_THREAD_H_

#include <functional>
#include <thread>
#include <memory>
#include <ff/ff_config.h>
#include <ff/Runnable.h>
#include <ff/Exception.h>
#include <ff/Noncopyable.h>

NS_FF_BEG

EXCEPTION_DEF2(ThreadException, RunnableException);

class LIBFF_API Thread: public Runnable, public Noncopyable {
public:
	Thread();
	Thread(RunnablePtr runnable);
	template<class Func> Thread(Func func) : Thread(MakeRunnable(func)){}
	virtual ~Thread();

	void start();
	void join();
	bool isJoinable() const;
	void detach();

	static void Sleep(unsigned int ms);
	static void (Yield)();
private:
	void run() override;

private:
	RunnablePtr m_runnable;
	std::thread m_thread;
};

typedef std::shared_ptr<Thread> ThreadPtr;

NS_FF_END

#endif /* FF_THREAD_H_ */
