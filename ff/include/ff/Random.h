/*
 * Random.h
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
 */

#ifndef FF_RANDOM_H_
#define FF_RANDOM_H_

#include <ff/ff_config.h>

namespace NS_FF {

/**
 * EXP:
 * 	Random ran;
 * 	uint32_t randomUint32Value = ran;
 * 	uint64_t randomUint64Value = ran;
 */

class Random {
public:
	Random();
	~Random();

	template<typename T>
	T random(){
		T t = 0;
		getRandomBytes(&t, sizeof(T));
		return t;
	}

	template<typename T>
	operator T() {
		T t = 0;
		getRandomBytes(&t, sizeof(T));
		return t;
	}

	void getRandomBytes(void *buf, uint32_t size);

private:
	int m_fd;
};

} /* namespace NS_FF */

#endif /* FF_RANDOM_H_ */
