/*
 * Runnable_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Runnable.h>
#include <gtest/gtest.h>
#include <iostream>
#include "TestDef.h"

using namespace std;
USE_NS_FF

void func1() {
	LOGD << __func__;
}

void func2(const std::string& info) {
	LOGD << __func__ << ": " << info;
}

TEST(TestRunnable, TestRunnable) {
	auto runnable = MakeRunnable(bind(&func1));
	runnable->run();

	runnable = MakeRunnable(bind(&func2, "this is a test text."));
	runnable->run();
}
