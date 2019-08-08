/*
 * Logger.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/Logger.h>

using namespace std;

namespace NS_FF {

Logger::Logger(const std::string& module) :
		m_module(module) {
}

Logger::~Logger() {
}

const std::string& Logger::getModule() const {
	return m_module;
}

void Logger::addAppender(AppenderPtr appender){
	lock_guard<mutex> lk(this->m_mutex);
	this->m_appenders.insert(appender);
}

void Logger::log(const LogInfo& logInfo){
	lock_guard<mutex> lk(this->m_mutex);
	for(auto& appender : this->m_appenders){
		appender->log(logInfo);
	}
}

} /* namespace NS_FF */

