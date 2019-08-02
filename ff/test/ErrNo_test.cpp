/*
 * ErrNo_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */
#include <gtest/gtest.h>
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
		cout << "last errno: " << GetLastErrNo() << endl;
		cout << "last err info: " << GetLastErrInfo() << endl;
	});

	thread t2([]() {
		SetLastError(1234, "err info 2");
		cout << "last errno: " << GetLastErrNo() << endl;
		cout << "last err info: " << GetLastErrInfo() << endl;
	});

	t1.join();
	t2.join();
}
