/*
 * Timer_test.cpp
 *
 *  Created on: 2021-05-31
 *      Author: ducky
 */

#include <ff/Tick.h>
#include <ff/Timer.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(TimerTest, TimerTest) {
  Timer tm;
  Tick t;

  auto intervalTaskId = tm.setInterval([t] { cout << "interval exec " << t.tock() << endl; }, 1000);
  cout << "id1: " << intervalTaskId << endl;

  cout << "id2: " << tm.setTimeout([t] { cout << "timeout exec2 " << t.tock() << endl; }, 5000) << endl;

  cout << "id3: " << tm.setTimeout(
      [t, &tm] {
        Sleep(5000);
        cout << "timeout exec3 " << t.tock() << endl;

        cout << "id5: " << tm.setTimeout([t] { cout << "timeout exec5 " << t.tock() << endl; },
                      6000) << endl;
      },
      500) << endl;

  cout << "id4: " << tm.setTimeout(
      [&tm, t, intervalTaskId] {
        Sleep(5000);
        cout << "timeout exec4 " << t.tock() << endl;
        tm.cancelTimeout(intervalTaskId);
      },
      100) << endl;

      cout << "=================" << endl;

  while (true) {
    this_thread::sleep_for(chrono::seconds(1));
  }
}
