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

NS_FF_BEG

class LIBFF_API Synchronizable {
public:
	Synchronizable();
	~Synchronizable();

	void lock();
	bool trylock();
	void unlock();

private:
	std::mutex m_mutex;
};

typedef NS_FF::Locker<Synchronizable> Synchronize;

NS_FF_END

#endif /* FF_SYNCHRONIZABLE_H_ */
