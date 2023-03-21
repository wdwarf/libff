/**
 * @file Tick_test.cpp
 * @auth DuckyLi
 * @date 2023-03-21 20:29:21
 * @description 
 */

#include <ff/Tick.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(TickTest, TickTest) {
  // timeBeginPeriod(1);
  for(int i = 0; i < 1000; ++i){
    Tick t;
    auto tt = Tick::GetTickCount();
    this_thread::sleep_for(chrono::milliseconds(10));
    LOGD << t.tock() << ", " << (Tick::GetTickCount() - tt);
  }
  // timeEndPeriod(1);
}
