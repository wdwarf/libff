/*
 * Semaphore.h
 *
 *  Created on: 2016-10-26
 *      Author: ducky
 */

#ifndef FF_SEMAPHORE_H_
#define FF_SEMAPHORE_H_

#include <string>
#include <ff/Object.h>
#include <semaphore.h>

namespace NS_FF {

using std::string;

class LIBFF_API Semaphore {
public:
	Semaphore();
	Semaphore(const string& name, int flag, ...);

	~Semaphore();

	int wait(int mSec = -1);
	int release();
	int getValue();
	void unlink();

private:
	sem_t* sem;
	string name;
	bool named;
};

} /* namespace NS_FF */

#endif /* FF_SEMAPHORE_H_ */
