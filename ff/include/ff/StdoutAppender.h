/*
 * StdoutAppender.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_STDOUTAPPENDER_H_
#define FF_STDOUTAPPENDER_H_

#include <ff/IAppender.h>

namespace NS_FF {

class StdoutAppender: public ff::IAppender {
public:
	StdoutAppender();
	virtual ~StdoutAppender();

	virtual void log(const LogInfo& logInfo) override;
};

} /* namespace NS_FF */

#endif /* FF_STDOUTAPPENDER_H_ */
