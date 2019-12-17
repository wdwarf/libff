/*
 * IAppender.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_IAPPENDER_H_
#define FF_IAPPENDER_H_

#include <memory>
#include <ff/ff_config.h>

namespace NS_FF {

class LogInfo;
class FFDLL IAppender {
public:
	IAppender() = default;
	virtual ~IAppender() = default;

	virtual void log(const LogInfo& logInfo) = 0;
};

typedef std::shared_ptr<IAppender> AppenderPtr;

} /* namespace NS_FF */

#endif /* FF_IAPPENDER_H_ */
