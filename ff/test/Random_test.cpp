/*
 * Random_test.cpp
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
 */

#include <gtest/gtest.h>
#include <set>
#include <ff/Random.h>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

TEST(RandomTest, RandomTest){
	Random ran;

	const int32_t genCnt = 1000;

	std::set<uint32_t> rand32;
	for(int i = 0; i < genCnt; ++i){
		uint32_t n = 0;
		ran.getRandomBytes(&n, sizeof(n));
		LOGD << "random uint32_t: " << n;
		EXPECT_EQ(rand32.end(), rand32.find(n));
		rand32.insert(n);
	}
	rand32.clear();
	for(int i = 0; i < genCnt; ++i){
		uint32_t n = ran.random<uint32_t>();
		LOGD << "random uint32_t: " << n;
		EXPECT_EQ(rand32.end(), rand32.find(n));
		rand32.insert(n);
	}
	rand32.clear();
	for(int i = 0; i < genCnt; ++i){
		uint32_t n = ran;
		LOGD << "random uint32_t: " << n;
		EXPECT_EQ(rand32.end(), rand32.find(n));
		rand32.insert(n);
	}

	std::set<uint32_t> rand64;
	for(int i = 0; i < genCnt; ++i){
		uint64_t n = 0;
		ran.getRandomBytes(&n, sizeof(n));
		LOGD << "random uint64_t: " << n;
		EXPECT_EQ(rand64.end(), rand64.find(n));
		rand32.insert(n);
	}
	rand64.clear();
	for(int i = 0; i < genCnt; ++i){
		uint64_t n = ran.random<uint64_t>();
		LOGD << "random uint64_t: " << n;
		EXPECT_EQ(rand64.end(), rand64.find(n));
		rand32.insert(n);
	}
	rand64.clear();
	for(int i = 0; i < genCnt; ++i){
		uint64_t n = ran;
		LOGD << "random uint64_t: " << n;
		EXPECT_EQ(rand64.end(), rand64.find(n));
		rand32.insert(n);
	}
}

