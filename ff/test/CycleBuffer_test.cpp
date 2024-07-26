/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Buffer.h>
#include <ff/CycleBuffer.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "TestDef.h"

using namespace std;

USE_NS_FF

TEST(CycleBufferTest, CycleBufferTest) {
  CycleBuffer<uint8_t> buf(8);
  ASSERT_EQ(buf.size(), 0);

  buf.put(1);
  ASSERT_EQ(buf.size(), 1);
  LOGD << "size: " << buf.size();
  uint8_t val = 0;
  buf.get(val);
  LOGD << "val: " << (uint16_t)val;
  ASSERT_EQ(val, 1);
  ASSERT_EQ(buf.size(), 0);
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();

  LOGD << "==============================";

  uint8_t data[] = {1, 2, 3, 4, 5, 6};
  buf.write(data, 6);
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
  LOGD << "==============================";

  uint8_t arr[16]{0};
  LOGD << buf.read(arr, 3);
  LOGD << Buffer(arr, 3).toHexString();
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
  LOGD << "==============================";

  for (uint8_t i = 10; i > 0; --i) buf.put(i);
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
  auto n = buf.read(arr, 30);
  LOGD << n;
  LOGD << Buffer(arr, n).toHexString();
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
}

TEST(CycleBufferTest, CycleBufferTest_int) {
  CycleBuffer<int32_t> buf(8);
  ASSERT_EQ(buf.size(), 0);
  buf.put(-1);
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
  int val = 0;
  buf.get(val);
  LOGD << "val: " << val;
  ASSERT_EQ(val, -1);
}

TEST(CycleBufferTest, CycleBufferTest_double) {
  CycleBuffer<double> buf(8);
  ASSERT_EQ(buf.size(), 0);
  buf.put(3.14159);
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
  double val = 0;
  buf.get(val);
  LOGD << "val: " << val;
  ASSERT_EQ(val, 3.14159);
}

TEST(CycleBufferTest, CycleBufferTest_str) {
  CycleBuffer<string> buf(8);
  ASSERT_EQ(buf.size(), 0);
  buf.put("str value");
  LOGD << "size: " << buf.size();
  LOGD << "pos: " << buf.pos();
  string val;
  buf.get(val);
  LOGD << "val: " << val;
  ASSERT_EQ(val, "str value");
}
