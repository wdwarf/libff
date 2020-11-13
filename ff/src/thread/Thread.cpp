/*
 * Thread.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/Thread.h>
#include <iostream>

using namespace std;

namespace NS_FF {

Thread::Thread() :
		m_runnable(shared_ptr<Thread>(this, [](void *) {})) {
}

Thread::Thread(RunnablePtr runnable) {
	this->m_runnable = runnable;
}

Thread::~Thread() {
	this->join();
}

void Thread::run() {
}

void Thread::start() {
	this->m_thread = thread([this] {
		try {
			this->m_runnable->run();
		} catch(std::exception& e) {
			throw;
		}
	});
}

void Thread::join() {
	if (this->isJoinable())
		this->m_thread.join();
}

bool Thread::isJoinable() const {
	return this->m_thread.joinable();
}

void Thread::detach() {
	this->m_thread.detach();
}

void Thread::Sleep(unsigned int ms) {
	this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void (Thread::Yield)() {
	this_thread::yield();
}

} /* namespace NS_FF */
