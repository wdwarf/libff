/*
 * LogInfo.h
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#ifndef FF_LOGINFO_H_
#define FF_LOGINFO_H_

#include <ctime>
#include <string>

#include <ff/Object.h>
//#include <ff/LogException.h>
#include <ff/DateTime.h>
#include <ff/Timestamp.h>

namespace NS_FF {

enum class LogLevel {
	VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL
};

std::string LogLevel2Str(LogLevel ll);

class LogType {
public:
	LogType();
	explicit LogType(const std::string& name);
	explicit LogType(const char* name);

	const std::string& getName() const;
	void setName(const std::string& name);

	operator std::string() const;
	LogType& operator=(const std::string& name);
	bool operator==(const std::string& name) const;
	bool operator==(const LogType& logType) const;
private:
	std::string m_name;
};

class LogInfo: public Object {
public:
	LogInfo();
	LogInfo(const LogLevel& logLevel, const std::string& logMessage,
			const std::string& logModule, const LogType& logType,
			const Timestamp& logTime, const std::string& fileName,
			const std::string& functionName, unsigned int lineNumber);
	virtual ~LogInfo();

	const std::string& getFileName() const;
	void setFileName(const std::string& fileName);
	const std::string& getFunctionName() const;
	void setFunctionName(const std::string& functionName);
	unsigned int getLineNumber() const;
	void setLineNumber(unsigned int lineNumber);
	const std::string& getLogMessage() const;
	void setLogMessage(const std::string& logMessage);
	LogLevel getLogLevel() const;
	void setLogLevel(LogLevel logLevel);
	const std::string& getLogModule() const;
	void setLogModule(const std::string& logModule);
	Timestamp getLogTime() const;
	void setLogTime(const Timestamp& logTime);
	const LogType& getLogType() const;
	void setLogType(const LogType& logType);

	std::string toXml() const;
	void fromXml(const std::string& xml);

	std::string toLogString() const;
private:
	LogLevel m_logLevel;
	std::string m_logMessage;
	std::string m_logModule;
	LogType m_logType;
	Timestamp m_logTime;

	std::string m_fileName;
	std::string m_functionName;
	unsigned int m_lineNumber;
};

#define MK_LOGINFO(logLevel, logMessage, logModule, logType)\
		LogInfo(logLevel, logMessage, logModule, logType, time(0), __FILE__, __FUNCTION__, __LINE__)

} /* namespace NS_FF */

#endif /* FF_LOGINFO_H_ */
