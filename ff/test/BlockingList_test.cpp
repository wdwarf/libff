/*
 * BlockingList_test.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: root
 */

#include <ff/BlockingList.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <thread>

using namespace std;
using namespace NS_FF;

TEST(TestBlockingList, TestBlockingList) {
	BlockingList<int> bl;

	thread t1([&] {
		int i = 0;
		while((i = bl.popFront()) > 0)
		{
			LOGD << "i : " << i << ", size: " << bl.size();
			this_thread::sleep_for(chrono::milliseconds(500));
		}
	});

	this_thread::sleep_for(chrono::seconds(1));
	int i = 5;
	while(i >= 0){
		bl.pushBack(i--);
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	t1.join();

	EXPECT_TRUE(bl.empty());
}

TEST(TestBlockingList, TestBlockingList2) {
	BlockingList<int> bl;

	thread t1([&] {
		int i = 0;
		while((i = bl.popBack()) > 0)
		{
			LOGD << "i : " << i << ", size: " << bl.size();
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	});

	this_thread::sleep_for(chrono::seconds(1));
	int i = 20;
	while(i > 0){
		bl.pushFront(i--);
		this_thread::sleep_for(chrono::milliseconds(1));
	}
	bl.pushFront(0);
	t1.join();
	EXPECT_TRUE(bl.empty());
}
