/*
 * Registry_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/windows/Registry.h>
#include <gtest/gtest.h>

#include <iostream>
#include <list>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

#ifdef _WIN32

TEST(RegistryTest, RegistryTest) {
  Registry reg;
  reg.open(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM\\", KEY_READ);
  list<RegistryValue> valueList = reg.enumStringValues();
  for (auto& val : valueList) {
    LOGD << "comm: " << val.asString();
  }
}

#endif