/*
 * StdoutAppender.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/StdoutAppender.h>
#include <ff/LogInfo.h>
#include <iostream>
#include <sstream>

using namespace std;

NS_FF_BEG

StdoutAppender::StdoutAppender() {
}

StdoutAppender::~StdoutAppender() {
}

void StdoutAppender::log(const LogInfo &logInfo) {
	ostream *o = &cout;
	if (logInfo.getLogLevel() >= LogLevel::Warning)
		o = &cerr;

	(*o) << logInfo.toLogString() << endl;
}

void StdoutAppender::log(const std::list<LogInfo>& logInfos){
	stringstream str;
	for(auto& logInfo : logInfos){
		str << logInfo.toLogString() << endl;
	}
	cout << str.str();
}

NS_FF_END
