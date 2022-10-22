/**
 * @file SHM_test.cpp
 * @auth DuckyLi
 * @date 2022-10-16 22:07:13
 * @description
 */

#include <ff/Application.h>
#include <ff/Process.h>
#include <ff/SHM.h>
#include <ff/Tick.h>
#include <gtest/gtest.h>

#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(SHMTest, SHMTest) {
  auto currDir = Application::GetApplicationDir();

  thread t1([currDir] {
    // system("shm_test master");

    try {
      string cmd = currDir + "/shm_test";
#ifdef WIN32
      cmd += ".exe";
#endif
      LOGD << cmd;
      Process proc(cmd);
      proc.addParameter("master");
      proc.setAsyncRead(false);
      proc.start();
      LOGD << "master start";
      char buf[1024];
      int len = 0;
      while ((len = proc.readData(buf, 1024)) > 0) {
        LOGD << string(buf, len);
      }
      proc.waitForFinished();
    } catch (const exception& e) {
      LOGE << e.what();
    }
  });

  thread t2([currDir] {
    // system("shm_test slave");
    try {
      string cmd = currDir + "/shm_test";
#ifdef WIN32
      cmd += ".exe";
#endif
      LOGD << cmd;
      Process proc(cmd);
      proc.addParameter("slave");
      proc.setAsyncRead(false);
      proc.start();
      LOGD << "slave start";
      char buf[1024];
      int len = 0;
      while ((len = proc.readData(buf, 1024)) > 0) {
        LOGD << string(buf, len);
      }
      proc.waitForFinished();
    } catch (const exception& e) {
      LOGE << e.what();
    }
  });

  t1.join();
  t2.join();
}
