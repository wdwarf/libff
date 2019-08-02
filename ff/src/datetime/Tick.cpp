/*
 * Tick.cpp
 *
 *  Created on: Nov 25, 2017
 *      Author: ducky
 */

#include <ff/Tick.h>
#include <ctime>
#include <cstring>
#include <errno.h>
#include <string>

using std::string;

namespace NS_FF {

Tick::Tick() :
		tick(0) {

}

Tick::~Tick() {

}

unsigned long Tick::GetTickCount() {
	struct timespec ts;
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		THROW_EXCEPTION(TickException,
				string("clock_gettime failed: ") + strerror(errno), errno);
	}
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

unsigned long Tick::start() {
	this->tick = Tick::GetTickCount();
	return this->tick;
}

unsigned long Tick::count() const {
	return (Tick::GetTickCount() - this->tick);
}

} /* namespace NS_FF */

