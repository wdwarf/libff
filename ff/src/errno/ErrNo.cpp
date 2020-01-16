/*
 * ErrNo.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/ErrNo.h>

using namespace std;

namespace NS_FF {

thread_local ErrorNo _threadErrNo = 0;
thread_local string _threadErrInfo;

void SetLastErr(ErrorNo errNo, const std::string& errInfo)
{
	_threadErrNo = errNo;
	_threadErrInfo = errInfo;
}

ErrorNo GetLastErr(){
	return _threadErrNo;
}

std::string& GetLastErrInfo(){
	return _threadErrInfo;
}

ErrorNo GetSysLastErr(){
#ifdef _WIN32
	return ::GetLastError();
#else
	return errno;
#endif
}

} /* namespace NS_FF */
