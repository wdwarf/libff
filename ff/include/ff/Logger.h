/*
 * Logger.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_LOGGER_H_
#define FF_LOGGER_H_

#include <string>
#include <memory>
#include <set>
#include <mutex>
#include <ff/Object.h>
#include <ff/IAppender.h>
#include <ff/LogInfo.h>
#include <ff/Noncopyable.h>

namespace NS_FF {

class LIBFF_API Logger: public Noncopyable {
public:
	Logger(const std::string& module,
			LogLevel logLevel = LogLevel::INFO);
	virtual ~Logger();

	const std::string& getModule() const;
	void addAppender(AppenderPtr appender);
	void log(const LogInfo& logInfo);
	LogLevel getLogLevel() const;
	void setLogLevel(LogLevel logLevel);

private:
	std::mutex m_mutex;
	std::string m_module;
	LogLevel m_logLevel;
	std::set<AppenderPtr> m_appenders;
};

typedef std::shared_ptr<Logger> LoggerPtr;

} /* namespace NS_FF */

#endif /* FF_LOGGER_H_ */
