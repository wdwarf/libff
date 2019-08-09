/*
 * StdoutAppender.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/StdoutAppender.h>
#include <ff/LogInfo.h>
#include <iostream>

using namespace std;

namespace NS_FF {

StdoutAppender::StdoutAppender() {
}

StdoutAppender::~StdoutAppender() {
}

void StdoutAppender::log(const LogInfo& logInfo) {
	ostream* o = &cout;
	if (logInfo.getLogLevel() >= LogLevel::WARNING)
		o = &cerr;

	(*o) << logInfo.toLogString() << endl;
}

} /* namespace NS_FF */
