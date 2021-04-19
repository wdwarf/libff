/*
 * Log_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <gtest/gtest.h>
#include <iostream>
#include <ff/Log.h>
#include <ff/StdoutAppender.h>

using namespace std;
USE_NS_FF

TEST(TestLog, TestLog){
	auto logger = Log::CreateLogger("test");
	logger->addAppender(make_shared<StdoutAppender>());

	Log("test").setFileName(__FILE__) << "this is a test text from log test.";
}
