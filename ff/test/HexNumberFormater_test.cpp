/**
 * @file HexNumberFormater_test.cpp
 * @author liyawu
 * @date 2025-01-13 15:16:31
 * @description
 */
/*
 * Curl_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/HexNumberFormater.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(HexNumberFormaterTest, HexNumberFormaterTest) {
  HexNumberFormater f;
  f.fillZero(true);
  f.usePrefix(true);
  f.uppercase(true);
  ASSERT_EQ("0xAB", f.format((uint8_t)0xabu));
  ASSERT_EQ("0xABCD", f.format((uint16_t)0xabcdu));
  ASSERT_EQ("0x0000ABCD", f.format((uint32_t)0xabcdu));
  ASSERT_EQ("0x89ABCDEF", f.format((uint32_t)0x89abcdefu));
  ASSERT_EQ("0xFFFF", f.format((uint16_t)65535));
  ASSERT_EQ("0xFFFFFFFF", f.format((uint32_t)-1));
  ASSERT_EQ("0xFFFFFFFFFFFFFFFF", f.format((uint64_t)-1));
  ASSERT_EQ(65535, f.parse("000000000000ffff"));
  ASSERT_EQ(65535, f.parse("0xFFff"));

  f.fillZero(false);
  f.usePrefix(true);
  f.uppercase(true);
  ASSERT_EQ("0xAB", f.format((uint8_t)0xabu));
  ASSERT_EQ("0xABCD", f.format((uint16_t)0xabcdu));
  ASSERT_EQ("0xABCD", f.format((uint32_t)0xabcdu));
  ASSERT_EQ("0x89ABCDEF", f.format((uint32_t)0x89abcdefu));
  ASSERT_EQ("0xFFFF", f.format((uint16_t)65535));
  ASSERT_EQ("0xFFFFFFFF", f.format((uint32_t)-1));
  ASSERT_EQ("0xFFFFFFFFFFFFFFFF", f.format((uint64_t)-1));
  ASSERT_EQ(65535, f.parse("000000000000ffff"));
  ASSERT_EQ(65535, f.parse("0xFFff"));

  f.fillZero(false);
  f.usePrefix(false);
  f.uppercase(true);
  ASSERT_EQ("AB", f.format((uint8_t)0xabu));
  ASSERT_EQ("ABCD", f.format((uint16_t)0xabcdu));
  ASSERT_EQ("ABCD", f.format((uint32_t)0xabcdu));
  ASSERT_EQ("89ABCDEF", f.format((uint32_t)0x89abcdefu));
  ASSERT_EQ("FFFF", f.format((uint16_t)65535));
  ASSERT_EQ("FFFFFFFF", f.format((uint32_t)-1));
  ASSERT_EQ("FFFFFFFFFFFFFFFF", f.format((uint64_t)-1));
  ASSERT_EQ(65535, f.parse("000000000000ffff"));
  ASSERT_EQ(65535, f.parse("0xFFff"));

  f.fillZero(false);
  f.usePrefix(false);
  f.uppercase(false);
  ASSERT_EQ("ab", f.format((uint8_t)0xabu));
  ASSERT_EQ("abcd", f.format((uint16_t)0xabcdu));
  ASSERT_EQ("abcd", f.format((uint32_t)0xabcdu));
  ASSERT_EQ("89abcdef", f.format((uint32_t)0x89abcdefu));
  ASSERT_EQ("ffff", f.format((uint16_t)65535));
  ASSERT_EQ("ffffffff", f.format((uint32_t)-1));
  ASSERT_EQ("ffffffffffffffff", f.format((uint64_t)-1));
  ASSERT_EQ(65535, f.parse("000000000000ffff"));
  ASSERT_EQ(65535, f.parse("0xFFff"));
}
