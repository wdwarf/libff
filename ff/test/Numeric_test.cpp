/**
 * @file Numeric_test.cpp
 * @author DuckyLi
 * @date 2023-06-29 21:34:02
 * @description
 */
#include <ff/Numeric.h>
#include <gtest/gtest.h>

#include <limits>

#include "TestDef.h"

using namespace std;
USE_NS_FF

#ifdef max
#undef max
#endif

TEST(NumericTest, NumericTest) {
  EXPECT_TRUE(IsEquals(10, 10));
  EXPECT_FALSE(IsEquals(12, 10));
  EXPECT_TRUE(IsEquals(std::numeric_limits<int>::max(),
                       std::numeric_limits<int>::max()));
  EXPECT_TRUE(IsEquals(10U, 10U));
  EXPECT_FALSE(IsEquals(12U, 10U));
  EXPECT_TRUE(IsEquals(std::numeric_limits<uint64_t>::max(),
                       std::numeric_limits<uint64_t>::max()));
  EXPECT_TRUE(IsEquals(10.123, 10.123));
  EXPECT_FALSE(IsEquals(12.12, 10.333));
  EXPECT_TRUE(IsEquals(3.1415926535897932384, 3.1415926535897928999));
  EXPECT_TRUE(IsEquals(std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max()));
  EXPECT_TRUE(IsEquals(std::numeric_limits<double>::max(),
                       std::numeric_limits<double>::max()));
}