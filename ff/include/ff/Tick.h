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

class Tick: public Object {
public:
	Tick();
	virtual ~Tick();

	unsigned long start();
	unsigned long count() const;

	static unsigned long GetTickCount();

private:
	unsigned long tick;
};

} /* namespace NS_FF */

#endif /* FF_TICK_H_ */
