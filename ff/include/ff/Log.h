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

class Log: public std::stringstream, public Object, public Noncopyable {
public:
	Log();
	Log(const std::string &module);
	virtual ~Log();

	static LoggerPtr CreateLogger(const std::string &module, LogLevel logLevel =
			LogLevel::INFO);
	static bool HasLogger(const std::string &module);
	static LoggerPtr GetLogger(const std::string &module);
	static void RemoveLogger(const std::string &module);

	void log(const LogInfo &logInfo);

	template<class T>
	Log& operator()(const T &logMsg) {
		(*this) << logMsg;
		return *this;
	}

	Log& operator()(const LogLevel &logLevel);

	Log& done();

	const LogLevel& getLogLevel() const;
	Log& setLogLevel(const LogLevel &logLevel);
	Log& setLogType(const std::string &logType);
	const std::string& getFileName() const;
	Log& setFileName(const std::string &fileName);
	const std::string& getFunctionName() const;
	Log& setFunctionName(const std::string &functionName);
	unsigned int getLineNumber() const;
	Log& setLineNumber(unsigned int lineNumber);

private:
	std::string m_module;
	LogLevel m_logLevel;
	std::string m_fileName;
	std::string m_functionName;
	unsigned int m_lineNumber;

private:
	typedef std::map<std::string, LoggerPtr> LoggerMap;
	static LoggerMap s_loggers;
	static std::mutex s_mutex;
};

} /* namespace NS_FF */

#endif /* FF_LOG_H_ */
