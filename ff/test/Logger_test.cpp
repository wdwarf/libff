/*
 * Logger_test.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/Logger.h>
#include <ff/StdoutAppender.h>
#include <ff/Tick.h>
#include <gtest/gtest.h>

#include <sstream>

#include "TestDef.h"

using namespace std;

NS_FF_BEG

#define RANGE(n, r) for (std::decay<decltype(r)>::type n = 0; n < r; ++n)

TEST(LoggerTest, LoggerTest) {
  stringstream str;
  string logModel = "ff_test_model";
  auto logger = Log::CreateLogger(logModel, LogLevel::Debug, true);
  logger->addAppender(make_shared<StdoutAppender>());
  const uint32_t logCnt = 100000;
  Tick t;
  RANGE(n, logCnt) {
    Log(logModel).setFileName(__FILE__).setFunctionName(__func__).setLineNumber(
        __LINE__)
        << "log message " << (n + 1);
  }
  str << "log tick: " << t.tock() << endl;
  logger->flush();
  str << "log flush tick: " << t.tock() << endl;
  cout << str.str() << endl;
  Log::RemoveLogger(logModel);
}

TEST(LoggerTest, LoggerTest2) {
  stringstream str;
  string logModel = "ff_test_model";
  auto logger = Log::CreateLogger(logModel, LogLevel::Debug, false);
  logger->addAppender(make_shared<StdoutAppender>());
  const uint32_t logCnt = 100000;
  Tick t;
  RANGE(n, logCnt) {
    Log(logModel).setFileName(__FILE__).setFunctionName(__func__).setLineNumber(
        __LINE__)
        << "log message " << (n + 1);
  }
  str << "log tick: " << t.tock() << endl;
  logger->flush();
  str << "log flush tick: " << t.tock() << endl;
  cout << str.str() << endl;
  Log::RemoveLogger(logModel);
}

NS_FF_END
