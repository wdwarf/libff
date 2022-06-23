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

  for (CrcAlgorithm a = CrcAlgorithm::Crc8; a <= CrcAlgorithm::Crc15Mpt1327;) {
    CrcCalculator crc(a);
    auto re = crc.calc(buf, strlen(buf));

    cout << CrcParams::instance().getCrcParamInfo(a)->getName() << ": 0x" << hex << uppercase
         << re << endl;

    a = (CrcAlgorithm)(uint32_t(a) + 1);
  }
}
