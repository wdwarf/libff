/*
 * FileLock.h
 *
 *  Created on: Jan 21, 2020
 *      Author: liyawu
 */

#ifndef FF_FILELOCK_H_
#define FF_FILELOCK_H_

#include <string>
#include <ff/ff_config.h>
#include <ff/Locker.h>

namespace NS_FF {

class FileLock {
public:
	FileLock(const std::string& file);
	~FileLock();

	void lock();
	bool trylock();
	void unlock();

	typedef ff::Locker<FileLock> Locker;
private:
	int m_fd;
};

} /* namespace NS_FF */

#endif /* FF_FILELOCK_H_ */
