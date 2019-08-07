/*
 * Thread_test.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <gtest/gtest.h>
#include <ff/Thread.h>
#include <iostream>

using namespace std;
using namespace NS_FF;

void threadFunc() {
	int n = 0;
	while (++n <= 3) {
		cout << __func__ << endl;
		this_thread::sleep_for(std::chrono::seconds(1));
	}
	cout << __func__ << " end." << endl;
}

TEST(TestThread, TestThread) {
	{
		Thread t(&threadFunc);
		t.start();
		t.detach();
		sleep(1);
	}
//	Thread(MakeRunnable(threadFunc)).start();

	cout << "test end" << endl;
}

