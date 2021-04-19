/*
 * Logger.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/Logger.h>

using namespace std;

NS_FF_BEG

Logger::Logger(const std::string& module, LogLevel logLevel) :
		m_module(module), m_logLevel(logLevel) {
}

Logger::~Logger() {
}

LogLevel Logger::getLogLevel() const {
	return m_logLevel;
}

void Logger::setLogLevel(LogLevel logLevel) {
	m_logLevel = logLevel;
}

const std::string& Logger::getModule() const {
	return m_module;
}

void Logger::addAppender(AppenderPtr appender){
	lock_guard<mutex> lk(this->m_mutex);
	this->m_appenders.insert(appender);
}

void Logger::log(const LogInfo& logInfo){
	if(logInfo.getLogLevel() < this->m_logLevel)
		return;

	lock_guard<mutex> lk(this->m_mutex);
	for(auto& appender : this->m_appenders){
		appender->log(logInfo);
	}
}

NS_FF_END
