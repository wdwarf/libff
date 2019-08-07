/*
 * ThreadPool.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/ThreadPool.h>
#include <functional>
#include <iostream>

using namespace std;
using namespace std::placeholders;

namespace NS_FF {

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
			cout << this->m_busyThreads.size() << endl;
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

void ThreadPool::exec(RunnablePtr task) {
	auto threadPtr = this->getThread();
	threadPtr->setTask(task);
}

unsigned int ThreadPool::getActiveThreadCount() {
	lock_guard<mutex> lk(this->m_mutex);
	return this->m_idelThreads.size() + this->m_busyThreads.size();
}

void ThreadPool::PutTaskThreadPtr(TaskThread* p) {
	lock_guard<mutex> lk(this->m_mutex);
	this->m_busyThreads.erase(p);
	this->m_idelThreads.insert(p);
}

TaskThread* ThreadPool::getThread() {
	auto getIdelThread = [&]() -> TaskThread* {
		lock_guard<mutex> lk(this->m_mutex);
		if(!this->m_idelThreads.empty())
		{
			auto it = this->m_idelThreads.begin();
			auto pThread = *it;
			this->m_idelThreads.erase(it);
			this->m_busyThreads.insert(pThread);
			return pThread;
		}
		return nullptr;
	};

	auto threadPtr = getIdelThread();
	if (threadPtr)
		return threadPtr;

	if (this->getActiveThreadCount() < this->m_maxSize) {
		lock_guard<mutex> lk(this->m_mutex);
		auto pThread = new TaskThread(this);
		pThread->start();
		this->m_busyThreads.insert(pThread);
		return pThread;
	}

	while (!(threadPtr = getIdelThread())) {
		Thread::Sleep(100);
	}

	return threadPtr;
}

} /* namespace NS_FF */
