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
		m_tick(Tick::GetTickCount()) {

}

Tick::~Tick() {

}

tick_t Tick::GetTickCount() {
	struct timespec ts;
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		THROW_EXCEPTION(TickException,
				string("clock_gettime failed: ") + strerror(errno), errno);
	}
	return (tick_t(ts.tv_sec * 1000) + ts.tv_nsec / 1000000);
}

tick_t Tick::start() {
	this->m_tick = Tick::GetTickCount();
	return this->m_tick;
}

tick_t Tick::count() const {
	return (Tick::GetTickCount() - this->m_tick);
}

} /* namespace NS_FF */

