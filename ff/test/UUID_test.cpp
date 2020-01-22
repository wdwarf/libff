/*
 * UUID_test.cpp
 *
 *  Created on: Jan 22, 2020
 *      Author: u16
 */

#include <ff/UUID.h>
#include <string>
#include <set>
#include <gtest/gtest.h>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

TEST(UUIDTest, UUIDTest){
	set<string> uuids;
	for(int i = 0; i < 100; ++i){
		string uuid = UUID();
		LOGD << uuid;
		EXPECT_EQ(uuids.end(), uuids.find(uuid));
		uuids.insert(uuid);
	}

	string uuidStr = "51a4fcbf-2254-4365-af5a-510b2bb8ab8e";
	UUID uuid(uuidStr);
	EXPECT_EQ(uuidStr, uuid.toString());
}
