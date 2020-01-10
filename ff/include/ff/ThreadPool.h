/*
 * ThreadPool.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_THREADPOOL_H_
#define FF_THREADPOOL_H_

#include <ff/Object.h>
#include <ff/Thread.h>
#include <ff/Noncopyable.h>
#include <mutex>
#include <condition_variable>
#include <set>
#include <atomic>

namespace NS_FF {

class TaskThread;
class FFDLL ThreadPool: public Noncopyable{
public:
	ThreadPool(unsigned int maxSize);
	virtual ~ThreadPool();

	void exec(RunnablePtr runnable);

	template<class Func> void exec(Func func){
		this->exec(MakeRunnable(func));
	}

	unsigned int getIdelThreadCount() const;
	unsigned int getActiveThreadCount() const;

private:
	friend class TaskThread;

	TaskThread* getThread();

	void PutTaskThreadPtr(TaskThread* p);
private:
	mutable std::mutex m_mutex;
	std::condition_variable m_cond;
	unsigned int m_maxSize;
	std::set<TaskThread*> m_idelThreads;
	std::set<TaskThread*> m_busyThreads;
};

typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;

} /* namespace NS_FF */

#endif /* FF_THREADPOOL_H_ */
