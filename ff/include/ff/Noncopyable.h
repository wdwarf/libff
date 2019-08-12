/*
 * Noncopyable.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_NONCOPYABLE_H_
#define FF_NONCOPYABLE_H_

#include <ff/ff_config.h>

namespace NS_FF {

class Noncopyable {
public:
	Noncopyable() = default;
	virtual ~Noncopyable() = default;

private:
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
};

} /* namespace NS_FF */

#endif /* FF_NONCOPYABLE_H_ */
