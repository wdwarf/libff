/*
 * ErrNo.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_ERRNO_H_
#define FF_ERRNO_H_

#include <string>
#include <ff/ff_config.h>

#ifdef _WIN32
#include <Windows.h>
typedef DWORD ErrorNo_t;
#else
#include <errno.h>
typedef error_t ErrorNo_t;
#endif

NS_FF_BEG

class LIBFF_API ErrNo {
public:
	static ErrNo LastError();

	ErrNo(ErrorNo_t errNo = 0);
	~ErrNo() = default;

	ErrorNo_t getErrorNo() const;
	operator ErrorNo_t() const;
	std::string getErrorStr() const;
	operator std::string() const;

private:
	ErrorNo_t m_errNo;
};

NS_FF_END

#endif /* FF_ERRNO_H_ */
