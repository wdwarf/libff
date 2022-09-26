/*
 * String_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/NetworkDevices.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(NetworkDevicesTest, NetworkDevicesTest) {
  auto devices = NetworkDevices::ListDevices();
  for (auto& dev : devices) {
    LOGD << "name: " << dev.name;
    LOGD << "desc: " << dev.desc;
    LOGD << "mac: " << dev.mac;
    for (auto& ip : dev.ips) {
      LOGD << "ip: " << ip.ip;
      LOGD << "ip type: " << (IpType::V4 == ip.type ? "v4" : "v6");
      LOGD << "mask: " << ip.mask;
      for (auto& gw : ip.gateways) {
        LOGD << "gw: " << gw;
      }
    }

    LOGD << "==========================";
  }
}
