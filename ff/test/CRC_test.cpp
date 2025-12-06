/*
 * CRC_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/CRC.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(CrcTest, CrcTest) {
  const char buf[] = "crc calulator test";

  CrcCalculator crc(CrcParameter::Create(CrcAlgorithm::Crc16Ibm));

  auto re = crc.calc(buf, strlen(buf));
  cout << "crc16-ibm: 0x" << hex << uppercase << re << dec << endl;
  EXPECT_EQ(re, 0xD0B3);
}
