/*
 * main.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Log.h>
#include <ff/StdoutAppender.h>
#include <gtest/gtest.h>

#include <iostream>

#include "TestDef.h"

using namespace std;
USE_NS_FF

void init();
void uninit();

static void SigHandler(int sig) {
#if defined(__linux) || defined(unix)
  cout << "catch signal: " << sig << endl;
#endif
}

static void SigCatch() {
#if defined(__linux) || defined(unix)
  int sigs[] = {SIGTERM, SIGPIPE};
  for (auto sig : sigs) {
    signal(sig, SigHandler);
  }
#endif
}

int main(int argc, char** argv) try {
  SigCatch();

  testing::InitGoogleTest(&argc, argv);

  init();

  int ret = RUN_ALL_TESTS();

  LOGD << "test end";
  uninit();
  return ret;
} catch (std::exception& e) {
  cerr << e.what() << endl;
  return -1;
} catch (...) {
  cerr << "Fatal error!!!!!!" << endl;
  return -1;
}

void init() {
  auto logger = Log::CreateLogger(TEST_LOG_MODULE, LogLevel::Debug, true);
  logger->addAppender(make_shared<StdoutAppender>());
}

void uninit() {
  LOGFLUSH();
  Log::RemoveLogger(TEST_LOG_MODULE);
}
