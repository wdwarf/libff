/*
 * Synchronizable.h
 *
 *  Created on: Dec 16, 2019
 *      Author: root
 */

#ifndef FF_SYNCHRONIZABLE_H_
#define FF_SYNCHRONIZABLE_H_

#include <ff/Object.h>
#include <mutex>

namespace NS_FF {

class FFDLL Synchronizable {
public:
	Synchronizable();
	virtual ~Synchronizable();

	void lock();
	void unlock();

private:
	std::mutex m_mutex;
};

class FFDLL Synchronize {
public:
	Synchronize(Synchronizable& sync);
	~Synchronize();

private:
	Synchronizable& m_sync;
};

} /* namespace NS_FF */

#endif /* FF_SYNCHRONIZABLE_H_ */
