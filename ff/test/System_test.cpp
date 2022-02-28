/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/System.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <iostream>
#include <vector>

using namespace std;
USE_NS_FF

TEST(SystemTest, SystemTest){
	LOGD << System::Hostname();
	auto memInfo = System::MemoryStatus();
	LOGD << "phyTotal:\t" << memInfo.phyTotal;
	LOGD << "phyFree:\t" << memInfo.phyFree;
	LOGD << "phyUsed:\t" << memInfo.phyUsed;
	LOGD << "swapTotal:\t" << memInfo.swapTotal;
	LOGD << "swapFree:\t" << memInfo.swapFree;
	LOGD << "swapUsed:\t" << memInfo.swapUsed;

	auto cpuInfo = System::CpuInfo();
	LOGD << cpuInfo.cpuId();
	LOGD << cpuInfo.vendor();
	LOGD << cpuInfo.brand();
	LOGD << cpuInfo.SSE3();
}
