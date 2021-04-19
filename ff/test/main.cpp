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

int main(int argc, char** argv){
	testing::InitGoogleTest(&argc, argv);

	init();

	return RUN_ALL_TESTS();
}

void init(){
	auto logger = Log::CreateLogger(TEST_LOG_MODULE, LogLevel::Debug);
	logger->addAppender(make_shared<StdoutAppender>());
}
