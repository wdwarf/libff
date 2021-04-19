/*
 * StdoutAppender.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_STDOUTAPPENDER_H_
#define FF_STDOUTAPPENDER_H_

#include <ff/ff_config.h>
#include <ff/IAppender.h>

NS_FF_BEG

class LIBFF_API StdoutAppender: public NS_FF::IAppender {
public:
	StdoutAppender();
	virtual ~StdoutAppender();

	virtual void log(const LogInfo& logInfo) override;
};

NS_FF_END

#endif /* FF_STDOUTAPPENDER_H_ */
