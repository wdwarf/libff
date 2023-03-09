/*
 * Thread_test.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/Delayable.h>
#include <ff/Thread.h>
#include <ff/Tick.h>
#include <gtest/gtest.h>

#include <iostream>

#include "TestDef.h"

using namespace std;
USE_NS_FF

void threadFunc() {
  LOGD << "thread id: " << Thread::CurrentThreadId();
  int n = 0;
  while (++n <= 3) {
    LOGD << __func__;
    this_thread::sleep_for(std::chrono::seconds(1));
  }
  LOGD << __func__ << " end.";
}

class Func {
 public:
  Func& operator()() {
    cout << "Functor operate" << endl;
    return *this;
  }
};

TEST(TestThread, TestThread) {
  {
    Thread t(&threadFunc);
    t.start();
    LOGD << "t id: " << t.id();
    t.detach();

    Thread t2((Func()));
    t2.start();
    t2.join();

    this_thread::sleep_for(chrono::seconds(1));
  }
  //	Thread(MakeRunnable(threadFunc)).start();

  LOGD << "test end";
}

TEST(TestThread, TestDelayable) {
  Delayable delay;
  Tick tick;
  tick.tick();
  int i = 0;
  atomic_bool end = false;
  thread t([&delay, &end] {
    this_thread::sleep_for(chrono::seconds(7));
    while (!end) {
      delay.cancel();
      this_thread::sleep_for(chrono::milliseconds(100));
    }
  });
  while (++i <= 10) {
    Tick t;
    delay.delay(3000);
    LOGD << i << ": " << t.tock();
  }
  end = true;
  t.join();
  LOGD << "TestDelayable tock: " << tick.tock();
}
