/*
 * ThreadPool_test.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */
#include <gtest/gtest.h>
#include <iostream>
#include <ff/ThreadPool.h>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

static void TestF(int n, int i){
	static mutex m;
	lock_guard<mutex> lk(m);
	LDBG << __func__ << n << ": " << i;
}

TEST(TestThreadPool, TestThreadPool) {
	ThreadPool tp(50);

	thread t1([&]{
		for(int i = 0; i < 100; ++i){
				tp.exec(MakeRunnable(bind(TestF, 1, i)));
			}
	});

	thread t2([&]{
		for(int i = 0; i < 100; ++i){
				tp.exec(MakeRunnable(bind(TestF, 2, i)));
			}
	});

	t1.join();
	t2.join();
}
