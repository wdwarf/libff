/*
 * Log.cpp
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#include <ff/Log.h>
#include <iostream>
#include <ff/DateTime.h>

using namespace std;

NS_FF_BEG

Log::LoggerMap Log::s_loggers;
std::mutex Log::s_mutex;

Log::Log() :
		m_logLevel(LogLevel::Info), m_lineNumber(0) {
}

Log::Log(const std::string &m_module) :
		m_logLevel(LogLevel::Info), m_lineNumber(0) {
	this->m_module = m_module;
}

Log::~Log() {
	this->done();
}

LoggerPtr Log::CreateLogger(const std::string &m_module, LogLevel logLevel) {
	if (Log::HasLogger(m_module)) {
		return Log::GetLogger(m_module);
	}

	lock_guard<mutex> lk(Log::s_mutex);
	LoggerPtr logger = make_shared<Logger>(m_module, logLevel);
	Log::s_loggers.insert(make_pair(m_module, logger));
	return logger;
}

bool Log::HasLogger(const std::string &m_module) {
	lock_guard<mutex> lk(Log::s_mutex);
	return (Log::s_loggers.find(m_module) != Log::s_loggers.end());
}

LoggerPtr Log::GetLogger(const std::string &m_module) {
	lock_guard<mutex> lk(Log::s_mutex);
	LoggerMap::iterator it = Log::s_loggers.find(m_module);
	if (it == Log::s_loggers.end()) {
		return LoggerPtr();
	}

	return it->second;
}

void Log::RemoveLogger(const std::string &m_module) {
	lock_guard<mutex> lk(Log::s_mutex);
	LoggerMap::iterator it = Log::s_loggers.find(m_module);
	if (it != Log::s_loggers.end()) {
		Log::s_loggers.erase(it);
	}
}

Log& Log::log(const LogInfo &logInfo) {
	try {
		LoggerPtr logger = Log::GetLogger(logInfo.getLogModule());
		if (!logger)
			return *this;

		logger->log(logInfo);
	} catch (Exception &e) {
		cout << e.what() << endl;
	}
	return *this;
}

Log& Log::operator()(const LogLevel &m_logLevel) {
	this->setLogLevel(m_logLevel);
	return *this;
}

Log& Log::done() {
	string logInfo = this->m_stream.str();

	if (logInfo.empty())
		return *this;

	this->m_stream.clear();
	this->m_stream.str("");

	this->log(
			LogInfo(this->m_logLevel, logInfo, this->m_module, Timestamp::now(),
					m_fileName, this->m_functionName, this->m_lineNumber));

	return *this;
}

const LogLevel& Log::getLogLevel() const {
	return m_logLevel;
}

Log& Log::setLogLevel(const LogLevel &logLevel) {
	this->m_logLevel = logLevel;
	return *this;
}

const std::string& Log::getFileName() const {
	return m_fileName;
}

Log& Log::setFileName(const std::string &fileName) {
	this->m_fileName = fileName;
	return *this;
}

const std::string& Log::getFunctionName() const {
	return m_functionName;
}

Log& Log::setFunctionName(const std::string &functionName) {
	this->m_functionName = functionName;
	return *this;
}

unsigned int Log::getLineNumber() const {
	return m_lineNumber;
}

Log& Log::setLineNumber(unsigned int lineNumber) {
	this->m_lineNumber = lineNumber;
	return *this;
}

NS_FF_END

