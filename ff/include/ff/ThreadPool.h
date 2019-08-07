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
#include <mutex>
#include <condition_variable>
#include <set>
#include <atomic>

namespace NS_FF {

class TaskThread;
typedef std::shared_ptr<TaskThread> TaskThreadPtr;

class ThreadPool: public ff::Object {
public:
	ThreadPool(unsigned int maxSize);
	virtual ~ThreadPool();

	void exec(RunnablePtr task);

private:
	friend class TaskThread;

	TaskThread* getThread();
	unsigned int getActiveThreadCount();

	void PutTaskThreadPtr(TaskThread* p);
private:
	std::mutex m_mutex;
	unsigned int m_maxSize;
	std::set<TaskThread*> m_idelThreads;
	std::set<TaskThread*> m_busyThreads;
};

class TaskThread : public Thread{
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

} /* namespace NS_FF */

#endif /* FF_THREADPOOL_H_ */
