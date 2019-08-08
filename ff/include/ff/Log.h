/*
 * Log.h
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#ifndef FF_LOG_H_
#define FF_LOG_H_

#include <map>
#include <string>
#include <sstream>
#include <mutex>
#include <ff/Object.h>
#include <ff/LogInfo.h>
#include <ff/Logger.h>
#include <ff/Variant.h>
#include <ff/StringWrapper.h>
#include <ff/Noncopyable.h>

namespace NS_FF {

class Log: public Object, public Noncopyable {
public:
	Log();
	Log(const std::string& module);
	virtual ~Log();

	static LoggerPtr CreateLogger(const std::string& module);
	static bool HasLogger(const std::string& module);
	static LoggerPtr GetLogger(const std::string& module);
	static void RemoveLogger(const std::string& module);

	void log(const LogInfo& logInfo);

	template<class T>
	Log& operator()(const T& logMsg){
		this->m_logBuffer << logMsg;
		return *this;
	}

	Log& operator()(const LogLevel& logLevel);
	Log& operator()(const LogType& logType);

	template<class T>
	Log& operator<<(const T& logMsg){
		this->m_logBuffer << logMsg;
		return *this;
	}

	Log& operator<<(const LogLevel& logLevel);

	Log& operator<<(const LogType& logType);
	Log& done(const LogType& type);
	Log& done(const std::string& type = "");

	const LogLevel& getLogLevel() const;
	Log& setLogLevel(const LogLevel& logLevel);
	const LogType& getLogType() const;
	Log& setLogType(const LogType& logType);
	Log& setLogType(const std::string& logType);
	const std::string& getFileName() const;
	Log& setFileName(const std::string& fileName);
	const std::string& getFunctionName() const;
	Log& setFunctionName(const std::string& functionName);
	unsigned int getLineNumber() const;
	Log& setLineNumber(unsigned int lineNumber);

private:
	std::string m_module;
	LogLevel m_logLevel;
	LogType m_logType;
	std::string m_fileName;
	std::string m_functionName;
	unsigned int m_lineNumber;
//	std::mutex m_mutex;
	LoggerPtr m_logger;
	std::stringstream m_logBuffer;

private:
	typedef std::map<std::string, LoggerPtr> LoggerMap;
	static LoggerMap s_loggers;
	static std::mutex s_mutex;
};

} /* namespace NS_FF */

#endif /* FF_LOG_H_ */
