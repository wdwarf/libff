/*
 * ErrNo.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_ERRNO_H_
#define FF_ERRNO_H_

#include <string>

namespace NS_FF {

void SetLastError(int errNo, const std::string& errInfo = "");
int GetLastErrNo();
std::string& GetLastErrInfo();

} /* namespace NS_FF */

#endif /* FF_ERRNO_H_ */
