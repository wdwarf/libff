#include <ff/SHM.h>
#include <ff/Tick.h>

#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace ff;

int main(int argc, char** argv) {
  string mode = "";
  if (argc > 1) {
    mode = argv[1];
  }

  if ("master" != mode && "slave" != mode) {
    cerr << "Invalid mode: " << mode << endl;
    return -1;
  }

  Tick tick;

  bool isMaster = ("master" == mode);

  SHMConnection conn;
  if(!conn.open("ff_sem_teset", 2048, isMaster)){
    cerr << mode << " SHM connection open failed" << endl;
    return -1;
  }

  cout << mode << " shm opened" << endl;

  uint32_t n = 0;
  uint32_t cnt = 1000000;

  thread t([&] {
    while (true) {
      stringstream str;
      str << (isMaster ? "master_" : "slave_") << ++n;
      auto s = str.str();
      conn.write(s.c_str(), s.length());
      if (n == cnt) break;
    }
  });

  uint32_t rn = 0;
  thread t2([&] {
    while (true) {
      char buf[128]{0};
      int len = conn.read(buf, 128);
      // if (len < 0) break;
      // if(len > 0)
      // cout << (string(buf, len) + "\n");

      // this_thread::sleep_for(chrono::seconds(1));

      if(rn % 10000 == 0) cout << mode << " read: " << string(buf, len) << endl;

      if(++rn == cnt)   break;
    }
  });

  t.join();
  t2.join();

  cout << mode << " read count: " << rn << endl;

  cout << mode << " tick: " << tick.tock() << endl;
  return 0;
}