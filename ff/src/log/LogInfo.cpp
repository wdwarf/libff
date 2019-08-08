/*
 * LogInfo.cpp
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#include <ff/LogInfo.h>

namespace NS_FF {

LogType::LogType() {
}

LogType::LogType(const std::string& name) {
	this->m_name = name;
}

LogType::LogType(const char* name) {
	if (NULL != name)
		this->m_name = name;
}

const std::string& LogType::getName() const {
	return m_name;
}

void LogType::setName(const std::string& name) {
	this->m_name = name;
}

LogType::operator std::string() const {
	return this->m_name;
}

LogType& LogType::operator=(const std::string& name) {
	this->m_name = name;
	return *this;
}

bool LogType::operator==(const std::string& name) const {
	return (this->m_name == name);
}

bool LogType::operator==(const LogType& logType) const {
	return (this->m_name == logType.m_name);
}

LogInfo::LogInfo() :
		m_logLevel(LogLevel::INFO), m_logTime(time(0)), m_lineNumber(0) {
	//
}

LogInfo::LogInfo(const LogLevel& _logLevel, const std::string& logMessage,
		const std::string& logModule, const LogType& logType, DateTime logTime,
		const std::string& fileName, const std::string& functionName,
		unsigned int lineNumber) :
		m_logLevel(LogLevel::INFO) {
	this->m_logLevel = _logLevel;
	this->m_logMessage = logMessage;
	this->m_logModule = logModule;
	this->m_logType = logType;
	this->m_logTime = logTime;
	this->m_fileName = fileName;
	this->m_functionName = functionName;
	this->m_lineNumber = lineNumber;
}

LogInfo::~LogInfo() {
	//
}

const std::string& LogInfo::getFileName() const {
	return m_fileName;
}

void LogInfo::setFileName(const std::string& fileName) {
	this->m_fileName = fileName;
}

const std::string& LogInfo::getFunctionName() const {
	return m_functionName;
}

void LogInfo::setFunctionName(const std::string& functionName) {
	this->m_functionName = functionName;
}

unsigned int LogInfo::getLineNumber() const {
	return m_lineNumber;
}

void LogInfo::setLineNumber(unsigned int lineNumber) {
	this->m_lineNumber = lineNumber;
}

LogLevel LogInfo::getLogLevel() const {
	return m_logLevel;
}

void LogInfo::setLogLevel(LogLevel logLevel) {
	this->m_logLevel = logLevel;
}

const std::string& LogInfo::getLogModule() const {
	return m_logModule;
}

void LogInfo::setLogModule(const std::string& logModule) {
	this->m_logModule = logModule;
}

DateTime LogInfo::getLogTime() const {
	return m_logTime;
}

void LogInfo::setLogTime(DateTime logTime) {
	this->m_logTime = logTime;
}

const LogType& LogInfo::getLogType() const {
	return m_logType;
}

void LogInfo::setLogType(const LogType& logType) {
	this->m_logType = logType;
}

const std::string& LogInfo::getLogMessage() const {
	return m_logMessage;
}

void LogInfo::setLogMessage(const std::string& logMessage) {
	this->m_logMessage = logMessage;
}

std::string LogInfo::toXml() const {
	return "";
}

void LogInfo::fromXml(const std::string& xml) {
	//
}

} /* namespace NS_FF */

