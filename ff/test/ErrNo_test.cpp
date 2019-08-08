/*
 * ErrNo_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */
#include <gtest/gtest.h>
#include "TestDef.h"
#include <ff/ErrNo.h>
#include <iostream>
#include <thread>
#include <errno.h>

using namespace std;
using namespace NS_FF;

void f() {
}

TEST(TestErrNo, TestErrNo) {
	thread t1([]() {
		SetLastError(123, "err info");
		this_thread::sleep_for(std::chrono::seconds(1));
		LDBG << "last errno: " << GetLastErrNo();
		LDBG << "last err info: " << GetLastErrInfo();
	});

	thread t2([]() {
		SetLastError(1234, "err info 2");
		LDBG << "last errno: " << GetLastErrNo();
		LDBG << "last err info: " << GetLastErrInfo();
	});

	t1.join();
	t2.join();
}
