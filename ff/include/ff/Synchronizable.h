/*
 * Synchronizable.h
 *
 *  Created on: Dec 16, 2019
 *      Author: root
 */

#ifndef FF_SYNCHRONIZABLE_H_
#define FF_SYNCHRONIZABLE_H_

#include <mutex>
#include <ff/Object.h>
#include <ff/Locker.h>

namespace NS_FF {

class FFDLL Synchronizable {
public:
	Synchronizable();
	~Synchronizable();

	void lock();
	bool trylock();
	void unlock();

private:
	std::mutex m_mutex;
};

typedef ff::Locker<Synchronizable> Synchronize;

} /* namespace NS_FF */

#endif /* FF_SYNCHRONIZABLE_H_ */
