/*
 * Random.h
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
 */

#ifndef FF_RANDOM_H_
#define FF_RANDOM_H_

#include <ff/ff_config.h>

#ifdef _WIN32
#include <Wincrypt.h>
#endif

namespace NS_FF {

/**
 * EXP:
 * 	Random ran;
 * 	uint32_t randomUint32Value = ran;
 * 	uint64_t randomUint64Value = ran;
 */

class LIBFF_API Random {
public:
	Random();
	~Random();

	template<typename T>
	T random(){
		T t;
		memset(&t, 0, sizeof(T));
		getRandomBytes(&t, sizeof(T));
		return t;
	}

	template<typename T>
	operator T() {
		T t;
		memset(&t, 0, sizeof(T));
		getRandomBytes(&t, sizeof(T));
		return t;
	}

	void getRandomBytes(void *buf, uint32_t size);

private:
#ifdef _WIN32
	HCRYPTPROV m_fd;
#else
	int m_fd;
#endif
};

} /* namespace NS_FF */

#endif /* FF_RANDOM_H_ */
