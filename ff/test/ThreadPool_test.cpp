/*
 * ThreadPool_test.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */
#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>
#include <ff/ThreadPool.h>
#include <ff/Tick.h>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

static void TestF(int n, int i){
	static mutex m;
	{
		lock_guard<mutex> lk(m);
		LOGD << __func__ << n << ": " << setw(3) << setfill('0') << i;
	}
	this_thread::sleep_for(std::chrono::seconds(2));
}

TEST(TestThreadPool, TestThreadPool) {
	ThreadPool tp(50);

	thread t1([&]{
		for(int i = 0; i < 100; ++i){
				tp.exec(MakeRunnable(bind(TestF, 1, i)));
				LOGD << "active thread cnt: " << tp.getActiveThreadCount();
				this_thread::yield();
			}
	});

	thread t2([&]{
		for(int i = 0; i < 100; ++i){
				tp.exec(MakeRunnable(bind(TestF, 2, i)));
				LOGD << "active thread cnt: " << tp.getActiveThreadCount();
				this_thread::yield();
			}
	});

	thread t3([&]{
		for(int i = 0; i < 100; ++i){
				tp.exec(MakeRunnable(bind(TestF, 3, i)));
				LOGD << "active thread cnt: " << tp.getActiveThreadCount();
				this_thread::yield();
			}
	});

	t1.join();
	t2.join();
	t3.join();
}

TEST(TestThreadPool, TestThreadPoolTimeout) {
	ThreadPool tp(1);

	tp.exec([]() {
		this_thread::sleep_for(std::chrono::seconds(5));
	});

	Tick tick;
	EXPECT_FALSE(tp.exec([]() { LOGD << "thread should not be executed."; }, 2000));
	LOGD << "tick: " << tick.tock();

	this_thread::sleep_for(std::chrono::milliseconds(100));
	LOGD << "begin thread 3";
	tick.tick();
	EXPECT_TRUE(tp.exec([]() { LOGD << "thread should be executed."; }, 5000));
	LOGD << "tick2: " << tick.tock();
}
