/*
 * ErrNo.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/ErrNo.h>

using namespace std;

namespace NS_FF {

thread_local int _threadErrNo = 0;
thread_local string _threadErrInfo;

void SetLastError(int errNo, const std::string& errInfo)
{
	_threadErrNo = errNo;
	_threadErrInfo = errInfo;
}

int GetLastErrNo(){
	return _threadErrNo;
}

std::string& GetLastErrInfo(){
	return _threadErrInfo;
}

} /* namespace NS_FF */
