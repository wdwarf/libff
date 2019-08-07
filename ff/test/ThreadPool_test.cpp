/*
 * ThreadPool_test.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */
#include <gtest/gtest.h>
#include <iostream>
#include <ff/ThreadPool.h>

using namespace std;
using namespace NS_FF;

static void TestF(int i){
	cout << __func__ << ": " << i << endl;
}

TEST(TestThreadPool, TestThreadPool) {
	ThreadPool tp(5);
	for(int i = 0; i < 100; ++i){
		tp.exec(MakeRunnable(bind(TestF, i)));
	}
}
