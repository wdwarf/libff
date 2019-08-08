/*
 * Log.cpp
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#include <ff/Log.h>
#include <iostream>
//#include <ff/LogException.h>
#include <ff/DateTime.h>

using namespace std;

namespace NS_FF {

Log::LoggerMap Log::s_loggers;
std::mutex Log::s_mutex;

Log::Log() :
		m_logLevel(LogLevel::INFO), m_lineNumber(0) {
}

Log::Log(const std::string& m_module) :
		m_logLevel(LogLevel::INFO) {
	this->m_module = m_module;
	this->m_logger = GetLogger(this->m_module);
}

Log::~Log() {
	this->done();
}

LoggerPtr Log::CreateLogger(const std::string& m_module) {
	if (Log::HasLogger(m_module)) {
		return Log::GetLogger(m_module);
	}

	lock_guard<mutex> lk(Log::s_mutex);
	LoggerPtr logger(new Logger(m_module));
	Log::s_loggers.insert(make_pair(m_module, logger));
	return logger;
}

bool Log::HasLogger(const std::string& m_module) {
	lock_guard<mutex> lk(Log::s_mutex);
	return (Log::s_loggers.find(m_module) != Log::s_loggers.end());
}

LoggerPtr Log::GetLogger(const std::string& m_module) {
	lock_guard<mutex> lk(Log::s_mutex);
	LoggerMap::iterator it = Log::s_loggers.find(m_module);
	if (it == Log::s_loggers.end()) {
		return LoggerPtr();
	}

	return it->second;
}

void Log::RemoveLogger(const std::string& m_module) {
	lock_guard<mutex> lk(Log::s_mutex);
	LoggerMap::iterator it = Log::s_loggers.find(m_module);
	if (it != Log::s_loggers.end()) {
		Log::s_loggers.erase(it);
	}
}

void Log::log(const LogInfo& logInfo) {
	try {
		LoggerPtr logger;
		if (m_module.empty()) {
			logger = Log::GetLogger(logInfo.getLogModule());
		} else {
			logger = this->m_logger;
		}
		logger->log(logInfo);
	} catch (Exception& e) {
		cout << e.what() << endl;
	}
}

Log& Log::operator()(const LogLevel& m_logLevel) {
	this->setLogLevel(m_logLevel);
	return *this;
}

Log& Log::operator()(const LogType& logType) {
	this->setLogType(logType);
	return *this;
}

Log& Log::operator<<(const LogLevel& m_logLevel) {
	this->setLogLevel(m_logLevel);
	return *this;
}

Log& Log::operator<<(const LogType& logType) {
	this->setLogType(logType);
	return *this;
}

Log& Log::done(const LogType& type) {
	string logInfo;
	{
//		lock_guard<mutex> lk(this->m_mutex);
		logInfo = this->m_logBuffer.str();

		if (logInfo.empty())
			return *this;
		this->m_logBuffer.clear();
		this->m_logBuffer.str("");
	}

	this->log(
			LogInfo(this->m_logLevel, logInfo, this->m_module,
					type.getName().empty() ? this->m_logType : type,
					DateTime::now(), m_fileName, this->m_functionName,
					this->m_lineNumber));

	return *this;
}

Log& Log::done(const std::string& type) {
	return this->done(LogType(type));
}

const LogLevel& Log::getLogLevel() const {
	return m_logLevel;
}

Log& Log::setLogLevel(const LogLevel& logLevel) {
//	lock_guard<mutex> lk(this->m_mutex);
	this->m_logLevel = logLevel;
	return *this;
}

const LogType& Log::getLogType() const {
	return m_logType;
}

Log& Log::setLogType(const LogType& logType) {
//	lock_guard<mutex> lk(this->m_mutex);
	this->m_logType = logType;
	return *this;
}

Log& Log::setLogType(const std::string& logType) {
	return this->setLogType(LogType(logType));
}

const std::string& Log::getFileName() const {
	return m_fileName;
}

Log& Log::setFileName(const std::string& fileName) {
	this->m_fileName = fileName;
	return *this;
}

const std::string& Log::getFunctionName() const {
	return m_functionName;
}

Log& Log::setFunctionName(const std::string& functionName) {
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

} /* namespace NS_FF */

