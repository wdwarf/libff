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
USE_NS_FF

TEST(TestBlockingList, TestBlockingList) {
	BlockingList<int> bl;

	thread t1([&] {
		int i = 0;
		while((i = bl.pop_front()) > 0)
		{
			LOGD << "i : " << i << ", size: " << bl.size();
			this_thread::sleep_for(chrono::milliseconds(500));
		}
	});

	this_thread::sleep_for(chrono::seconds(1));
	int i = 5;
	while(i >= 0){
		bl.push_back(i--);
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	t1.join();

	EXPECT_TRUE(bl.empty());
}

TEST(TestBlockingList, TestBlockingList2) {
	BlockingList<int> bl;

	thread t1([&] {
		int i = 0;
		while((i = bl.pop_back()) > 0)
		{
			LOGD << "i : " << i << ", size: " << bl.size();
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	});

	this_thread::sleep_for(chrono::seconds(1));
	int i = 20;
	while(i > 0){
		bl.push_front(i--);
		this_thread::sleep_for(chrono::milliseconds(1));
	}
	bl.push_front(0);
	t1.join();
	EXPECT_TRUE(bl.empty());
}
