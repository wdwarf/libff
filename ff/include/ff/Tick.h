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

namespace NS_FF {

EXCEPTION_DEF(TickException);
typedef long long tick_t;

class LIBFF_API Tick {
public:
	Tick();
	virtual ~Tick();

	tick_t start() _throws(TickException);
	tick_t count() const _throws(TickException);

	static tick_t GetTickCount() _throws(TickException);

private:
	tick_t m_tick;
};

} /* namespace NS_FF */

#endif /* FF_TICK_H_ */
