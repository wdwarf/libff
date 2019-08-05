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

namespace NS_FF {

Semaphore::Semaphore() :
		sem(new sem_t), named(false) {
	
	sem_init(this->sem, 0, 0);
}

Semaphore::Semaphore(const string& name, int flag, ...) :
		named(true) {
	va_list ap;
	va_start(ap, flag);
	this->sem = sem_open(name.c_str(), flag, ap);
	va_end(ap);
	this->name = name;
}

Semaphore::~Semaphore() {
	
	if (this->named) {
		sem_close(this->sem);
	} else {
		sem_destroy(this->sem);
		delete this->sem;
	}
}

int Semaphore::wait(int mSec) {
	if (mSec < 0) {
		return sem_wait(this->sem);
	} else {
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += mSec / 1000;
		ts.tv_nsec += (mSec % 1000) * 1000000;
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_sec += 1;
			ts.tv_nsec -= 1000000000;
		}
		return sem_timedwait(this->sem, &ts);
	}
}

int Semaphore::release() {
	return sem_post(this->sem);
}

int Semaphore::getValue() {
	int re = 0;
	sem_getvalue(this->sem, &re);
	return re;
}

void Semaphore::unlink() {
	if (this->named) {
		sem_unlink(this->name.c_str());
	}
}

} /* namespace NS_FF */

