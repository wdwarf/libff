/**
 * @file SHM_test.cpp
 * @auth DuckyLi
 * @date 2022-10-16 22:07:13
 * @description
 */

#include <ff/SHM.h>
#include <gtest/gtest.h>
#include <ff/Tick.h>

#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(SHMTest, SHMTest) {
  auto pid = fork();
  if (0 == pid) {
    this_thread::sleep_for(chrono::seconds(1));
    SHM shm;
    char* d = (char*)shm.create("ff_shm_test_name", 4096);
    cout << "shm: " << string(d, 3) << endl;
    exit(0);
  }

  SHM shm;
  char* data = (char*)shm.create("ff_shm_test_name", 4096);

  memcpy(data, "123", 3);
  this_thread::sleep_for(chrono::seconds(1));
  cout << "end" << endl;
}

TEST(SHMTest, SHMConnectionTest) {
  Tick tick;
  bool master = true;

  auto pid = fork();
  if (0 == pid) {
    master = false;
  }

  SHMConnection conn;
  conn.open("ff_sem_teset", 2048, master);

  uint32_t n = 0;
  uint32_t cnt = 1000000;

  thread t([&] {
    while (true) {
      stringstream str;
      str << (master ? "master_" : "slave_") << ++n;
      auto s = str.str();
      conn.write(s.c_str(), s.length());
      if (n == cnt) break;
    }
  });

  uint32_t rn = 0;
  thread t2([&] {
    while (true) {
      char buf[128]{0};
      int len = conn.read(buf, 128, 1000);
      if (len < 0) break;
      cout << (string(buf, len) + "\n");
      // this_thread::sleep_for(chrono::seconds(1));
      // if(++rn == cnt)   break;
    }
  });

  t.join();
  t2.join();

  // if(master){
  //   while(true) this_thread::sleep_for(chrono::seconds(1));
  // }
  if (0 == pid) exit(0);
  cout << "tick: " << tick.tock() << endl;
}
