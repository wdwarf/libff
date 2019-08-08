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

namespace NS_FF {

class Logger: public ff::Object {
public:
	Logger(const std::string& module);
	virtual ~Logger();

	const std::string& getModule() const;
	void addAppender(AppenderPtr appender);
	void log(const LogInfo& logInfo);

private:
	std::mutex m_mutex;
	std::string m_module;
	std::set<AppenderPtr> m_appenders;
};

typedef std::shared_ptr<Logger> LoggerPtr;

} /* namespace NS_FF */

#endif /* FF_LOGGER_H_ */
