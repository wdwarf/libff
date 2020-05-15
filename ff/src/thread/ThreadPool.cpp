/*
 * ThreadPool.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/ThreadPool.h>
#include <functional>

using namespace std;
using namespace std::placeholders;

namespace NS_FF {

class TaskThread: public Thread {
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

TaskThread::TaskThread(ThreadPool* threadPool) :
		m_threadPool(threadPool), m_exit(false) {

}

TaskThread::~TaskThread() {
	this->join();
}

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

		if (!this->m_task)
			continue;

		try {
			this->m_task->run();
			this->m_task.reset();
			this->m_threadPool->PutTaskThreadPtr(this);
		} catch (std::exception& e) {
			throw;
		}
	}
}

ThreadPool::ThreadPool(unsigned int maxSzie) :
		m_maxSize(maxSzie) {
}

ThreadPool::~ThreadPool() {
	while (true) {
		{
			lock_guard<mutex> lk(this->m_mutex);
			if (this->m_busyThreads.empty())
				break;
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
	if (!threadPtr)
		return false;
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

	if (this->m_idelThreads.size() + this->m_busyThreads.size()
			< this->m_maxSize) {
		auto pThread = new TaskThread(this);
		pThread->start();
		this->m_busyThreads.insert(pThread);
		return pThread;
	}

	if (timeoutMs >= 0) {
		if (!this->m_cond.wait_for(lk, std::chrono::milliseconds(timeoutMs),
				[&] {return !this->m_idelThreads.empty();}))
			return nullptr;
	} else
		this->m_cond.wait(lk, [&] {return !this->m_idelThreads.empty();});

	auto it = this->m_idelThreads.begin();
	auto pThread = *it;
	this->m_idelThreads.erase(it);
	this->m_busyThreads.insert(pThread);
	return pThread;

}

void ThreadPool::waitAll()
{
	unique_lock<mutex> lk(this->m_mutex);
	this->m_cond.wait(lk, [&] {return this->m_busyThreads.empty(); });
}

} /* namespace NS_FF */
