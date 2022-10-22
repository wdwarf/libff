/*
 * main.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <gtest/gtest.h>
#include <iostream>
#include "TestDef.h"
#include <ff/Log.h>
#include <ff/StdoutAppender.h>

using namespace std;
USE_NS_FF

void init();

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

int main(int argc, char** argv){
	SigCatch();

	testing::InitGoogleTest(&argc, argv);

	init();

	return RUN_ALL_TESTS();
}

void init(){
	auto logger = Log::CreateLogger(TEST_LOG_MODULE, LogLevel::Debug);
	logger->addAppender(make_shared<StdoutAppender>());
}
