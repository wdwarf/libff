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
typedef DWORD ErrorNo;
#else
#include <errno.h>
typedef error_t ErrorNo;
#endif

namespace NS_FF {

FFDLL void SetLastErr(ErrorNo errNo, const std::string& errInfo = "");
FFDLL ErrorNo GetLastErr();
FFDLL std::string& GetLastErrInfo();

FFDLL ErrorNo GetSysLastErr();

} /* namespace NS_FF */

#endif /* FF_ERRNO_H_ */
