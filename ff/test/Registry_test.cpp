/*
 * Registry_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifdef _WIN32

#include <ff/windows/Registry.h>
#include <gtest/gtest.h>

#include <iostream>
#include <list>
#include <vector>

#include "TestDef.h"

using namespace std;

USE_NS_FF

TEST(RegistryTest, RegistryTest) {
  Registry reg;
  reg.open(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM\\", KEY_READ);
  list<RegistryValue> valueList = reg.enumStringValues();
  for (auto& val : valueList) {
    LOGD << "comm: " << val.asString();
  }
}

#endif
