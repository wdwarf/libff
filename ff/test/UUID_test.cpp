/*
 * UUID_test.cpp
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
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
		string uuid = Uuid();
		LOGD << uuid;
		EXPECT_EQ(uuids.end(), uuids.find(uuid));
		uuids.insert(uuid);
	}

	string uuidStr = "51a4fcbf-2254-4365-af5a-510b2bb8ab8e";
	Uuid uuid(uuidStr);
	EXPECT_EQ(uuidStr, uuid.toString());
}
