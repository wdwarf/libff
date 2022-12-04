/*
 * Logger.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_LOGGER_H_
#define FF_LOGGER_H_

#include <ff/IAppender.h>
#include <ff/LogInfo.h>
#include <ff/Noncopyable.h>
#include <ff/Object.h>

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

NS_FF_BEG

class LIBFF_API Logger : public Noncopyable {
 public:
  Logger(const std::string& module, LogLevel logLevel = LogLevel::Info, bool threading = false);
  ~Logger();

  const std::string& getModule() const;
  void addAppender(AppenderPtr appender);
  void log(const LogInfo& logInfo);
  LogLevel getLogLevel() const;
  void setLogLevel(LogLevel logLevel);

  void flush();
	void start();
  void stop();

 private:
  std::mutex m_mutexLog;
  std::mutex m_mutexAppender;
  std::string m_module;
  LogLevel m_logLevel;
  std::set<AppenderPtr> m_appenders;

  std::atomic_bool m_stoped;
  std::atomic_bool m_threading;
  std::thread m_logThread;
  std::condition_variable m_cond;
  std::list<LogInfo> m_logInfos;
};

typedef std::shared_ptr<Logger> LoggerPtr;

NS_FF_END

#endif /* FF_LOGGER_H_ */
