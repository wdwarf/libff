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

NS_FF_BEG

Tick::Tick() :
		m_tick(Tick::GetTickCount()) {

}

Tick::~Tick() {

}

tick_t Tick::GetTickCount() {
#ifdef _WIN32
	return ::GetTickCount();
#else
	struct timespec ts;
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		THROW_EXCEPTION(TickException,
				string("clock_gettime failed: ") + strerror(errno), errno);
	}
	return (tick_t(ts.tv_sec * 1000) + ts.tv_nsec / 1000000);
#endif
}

tick_t Tick::tick() {
	return (this->m_tick = Tick::GetTickCount());
}

tick_t Tick::tock() const {
	return (Tick::GetTickCount() - this->m_tick);
}

NS_FF_END

