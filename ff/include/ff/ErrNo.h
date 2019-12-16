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

namespace NS_FF {

FFDLL void SetLastError(int errNo, const std::string& errInfo = "");
FFDLL int GetLastErrNo();
FFDLL std::string& GetLastErrInfo();

} /* namespace NS_FF */

#endif /* FF_ERRNO_H_ */
