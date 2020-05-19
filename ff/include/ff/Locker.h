/*
 * Locker.h
 *
 *  Created on: Jan 21, 2020
 *      Author: liyawu
 */

#ifndef FF_LOCKER_H_
#define FF_LOCKER_H_

#include <ff/ff_config.h>

namespace NS_FF {

template<class L>
class Locker {
public:
	Locker(L& lock) : m_lock(lock){
		m_lock.lock();
	}

	~Locker(){
		m_lock.unlock();
	}

private:
	L& m_lock;
};

} /* namespace NS_FF */

#endif /* FF_LOCKER_H_ */
