/*
 * Logger.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/Logger.h>

using namespace std;

NS_FF_BEG

Logger::Logger(const std::string& module, LogLevel logLevel, bool threading)
    : m_module(module),
      m_logLevel(logLevel),
      m_threading(threading),
      m_stoped(false) {}

void Logger::start() {
  if (!this->m_threading) return;
  this->m_logThread = thread([this] {
    while (!this->m_stoped) {
      std::list<ff::LogInfo> logInfos;
      {
        unique_lock<mutex> lk(this->m_mutexLog);
        if (this->m_logInfos.empty()) {
          this->m_cond.wait(lk);
          if (m_stoped) break;
          if (this->m_logInfos.empty()) continue;
        }

        logInfos = std::move(this->m_logInfos);
      }

      if (logInfos.empty()) continue;

      lock_guard<mutex> lkA(this->m_mutexAppender);
      for (auto& logInfo : logInfos) {
        for (auto& appender : this->m_appenders) {
          appender->log(logInfo);
        }
      }
    }
  });
}

void Logger::stop() {
  if (this->m_stoped) return;
  this->m_stoped = true;
  this->m_cond.notify_one();
  if (this->m_logThread.joinable()) this->m_logThread.join();
}

Logger::~Logger() { this->stop(); }

LogLevel Logger::getLogLevel() const { return m_logLevel; }

void Logger::setLogLevel(LogLevel logLevel) { m_logLevel = logLevel; }

const std::string& Logger::getModule() const { return m_module; }

void Logger::addAppender(AppenderPtr appender) {
  lock_guard<mutex> lk(this->m_mutexAppender);
  this->m_appenders.insert(appender);
}

void Logger::log(const LogInfo& logInfo) {
  if (logInfo.getLogLevel() < this->m_logLevel) return;

  if (m_threading) {
    lock_guard<mutex> lk(this->m_mutexLog);
    this->m_logInfos.push_back(logInfo);
    this->m_cond.notify_one();
    return;
  }

  lock_guard<mutex> lk(this->m_mutexAppender);
  for (auto& appender : this->m_appenders) {
    appender->log(logInfo);
  }
}

void Logger::flush() {
  lock_guard<mutex> lkL(this->m_mutexLog);
  lock_guard<mutex> lkA(this->m_mutexAppender);
  for (auto& logInfo : this->m_logInfos) {
    for (auto& appender : this->m_appenders) {
      appender->log(logInfo);
    }
  }
  this->m_logInfos.clear();
}

NS_FF_END
