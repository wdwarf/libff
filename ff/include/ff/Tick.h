/*
 * Tick.h
 *
 *  Created on: Nov 25, 2017
 *      Author: ducky
 */

#ifndef FF_TICK_H_
#define FF_TICK_H_

#include <ff/Object.h>
#include <ff/Exception.h>

NS_FF_BEG

typedef long long tick_t;

class LIBFF_API Tick {
public:
	Tick();
	~Tick();

	tick_t tick();
	tick_t tock() const;

	static tick_t GetTickCount();

private:
	class TickImpl;
	TickImpl* m_impl;
};

NS_FF_END

#endif /* FF_TICK_H_ */
