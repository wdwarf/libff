/**
 * @file Snowflake_test.cpp
 * @auth DuckyLi
 * @date 2022-04-06 15:34:47
 * @description
 */

#include <ff/Snowflake.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(SnowflakeTest, SnowflakeTest) {
  Snowflake sf(1, 1);
  uint64_t lastId = 0;
  for (uint32_t i = 0; i < 10000; ++i) {
    auto id = sf.gen();
    LOGD << bitset<sizeof(id) * 8>(id) << " : " << id;
    ASSERT_GT(id, lastId);
    lastId = id;
  }

  Snowflake sf2(sf.lastTimestamp(), sf.lastSequence(), sf.datacenterId(),
                sf.workerId());
  for (uint32_t i = 0; i < 10000; ++i) {
    auto id = sf2.gen();
    LOGD << bitset<sizeof(id) * 8>(id) << " : " << id;
    ASSERT_GT(id, lastId);
    lastId = id;
  }
}
