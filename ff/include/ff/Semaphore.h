/*
 * Semaphore.h
 *
 *  Created on: 2016-10-26
 *      Author: ducky
 */

#ifndef FF_SEMAPHORE_H_
#define FF_SEMAPHORE_H_

#include <string>
#include <ff/ff_config.h>
#include <ff/Object.h>
#ifdef _WIN32
#else
#include <semaphore.h>
#endif

NS_FF_BEG

using std::string;

class LIBFF_API Semaphore {
public:
	Semaphore(int value = 0);
	Semaphore(const string& name, int value = 0);

	~Semaphore();

	bool wait(int mSec = -1);
	void release();
	int getValue();
	void unlink();

private:
#ifdef _WIN32
	HANDLE m_sem;
	LONG m_previousCount;
#else
	sem_t* m_sem;
#endif
	string m_name;
	bool m_named;
};

NS_FF_END

#endif /* FF_SEMAPHORE_H_ */
