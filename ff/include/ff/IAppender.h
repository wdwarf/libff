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
#include <ff/Object.h>

NS_FF_BEG

class LogInfo;
class LIBFF_API IAppender : public NS_FF::Object {
public:
	IAppender() = default;
	virtual ~IAppender() = default;

	virtual void log(const LogInfo& logInfo) = 0;
};

typedef std::shared_ptr<IAppender> AppenderPtr;

NS_FF_END

#endif /* FF_IAPPENDER_H_ */
