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

NS_FF_BEG

class LIBFF_API FileLock {
public:
	FileLock(const std::string& file);
	~FileLock();

	void lock();
	bool trylock();
	void unlock();

	typedef NS_FF::Locker<FileLock> Locker;
private:
#ifdef _WIN32
	HANDLE m_fd;
#else
	int m_fd;
#endif
};

NS_FF_END

#endif /* FF_FILELOCK_H_ */
