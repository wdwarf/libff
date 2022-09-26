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
#include <set>

using namespace std;
USE_NS_FF

TEST(SystemTest, SystemTest){
	LOGD << System::Hostname();
	LOGD << "username: " << System::Username();
	auto memInfo = System::MemoryStatus();
	LOGD << "phyTotal:\t" << memInfo.phyTotal;
	LOGD << "phyFree:\t" << memInfo.phyFree;
	LOGD << "phyUsed:\t" << memInfo.phyUsed;
	LOGD << "swapTotal:\t" << memInfo.swapTotal;
	LOGD << "swapFree:\t" << memInfo.swapFree;
	LOGD << "swapUsed:\t" << memInfo.swapUsed;

	auto cpuInfo = System::CpuInfo();
	LOGD << "cpuid: " << cpuInfo.cpuId() << ", cores: " << cpuInfo.cores() 
		<< ", clock(MHz): " << cpuInfo.clock()
		<< ", cache size: " << cpuInfo.cacheSize();
	LOGD << "vendor: " << cpuInfo.vendor();
	LOGD << "brand: " << cpuInfo.brand();
	LOGD << "SSE3 suported: " << cpuInfo.SSE3();
	LOGD << "Family: " << cpuInfo.family() << ", Model: " << cpuInfo.model() << ", Stepping ID: " << cpuInfo.steppingId();
	LOGD << "ext family: " << cpuInfo.extendedFamily() << ", ext model: " << cpuInfo.extendedModel() << ", model synth: " << ((cpuInfo.extendedModel() << 4) | cpuInfo.model());
}
