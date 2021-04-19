/*
 * Semaphore.cpp
 *
 *  Created on: 2016-10-26
 *      Author: liyawu
 */

#include <ff/Semaphore.h>
#include <stdarg.h>
#include <iostream>
using namespace std;

NS_FF_BEG

Semaphore::Semaphore() : m_named(false) {
#ifdef _WIN32
	this->m_previousCount = 0;
	this->m_sem = ::CreateSemaphore(NULL, 0, 0, NULL);
#else
	this->m_sem = new sem_t,
	sem_init(this->m_sem, 0, 0);
#endif
}

Semaphore::Semaphore(const string& name, int flag, ...) :
		m_named(true) {
#ifdef _WIN32
	this->m_previousCount = 0;
	this->m_sem = ::CreateSemaphore(NULL, 0, 0, name.c_str());
#else
	va_list ap;
	va_start(ap, flag);
	this->m_sem = sem_open(name.c_str(), flag, ap);
	va_end(ap);
	this->m_name = name;
#endif
}

Semaphore::~Semaphore() {
#ifdef _WIN32
	::CloseHandle(this->m_sem);
#else
	if (this->m_named) {
		sem_close(this->m_sem);
}
	else {
		sem_destroy(this->m_sem);
		delete this->m_sem;
	}
#endif
}

void Semaphore::wait(int mSec) {
#ifdef _WIN32
	(void)::WaitForSingleObject(this->m_sem, mSec);
#else
	if (mSec < 0) {
		(void)sem_wait(this->m_sem);
		return;
	}

	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += mSec / 1000;
	ts.tv_nsec += (mSec % 1000) * 1000000;
	if (ts.tv_nsec >= 1000000000) {
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}
	
	(void)sem_timedwait(this->m_sem, &ts);
#endif
}

void Semaphore::release() {
#ifdef _WIN32
	(void)::ReleaseSemaphore(this->m_sem, 1, &this->m_previousCount);
#else
	(void)sem_post(this->m_sem);
#endif
}

int Semaphore::getValue() {
#ifdef _WIN32
	return this->m_previousCount;
#else
	int re = 0;
	sem_getvalue(this->m_sem, &re);
	return re;
#endif
}

void Semaphore::unlink() {
#ifdef _WIN32
#else
	if (this->m_named) {
		sem_unlink(this->m_name.c_str());
	}
#endif
}

NS_FF_END

