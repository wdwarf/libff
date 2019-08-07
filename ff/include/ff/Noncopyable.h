/*
 * Noncopyable.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_NONCOPYABLE_H_
#define FF_NONCOPYABLE_H_

namespace NS_FF {

class Noncopyable {
public:
	Noncopyable() = default;
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
	virtual ~Noncopyable() = default;
};

} /* namespace NS_FF */

#endif /* FF_NONCOPYABLE_H_ */
