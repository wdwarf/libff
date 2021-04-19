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

EXCEPTION_DEF(TickException);
typedef long long tick_t;

class LIBFF_API Tick {
public:
	Tick();
	virtual ~Tick();

	tick_t tick() _throws(TickException);
	tick_t tock() const _throws(TickException);

	static tick_t GetTickCount() _throws(TickException);

private:
	tick_t m_tick;
};

NS_FF_END

#endif /* FF_TICK_H_ */
