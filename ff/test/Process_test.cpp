/**
 * @file Process_test.cpp
 * @author DuckyLi
 * @date 2023-07-24 21:41:49
 * @description
 */

#include <ff/Process.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(ProcessTest, ProcessTest) {
#ifdef _WIN32
  Process process("cmd");
  process.setAsyncRead(false);
  process.start();

  string cmd = "dir\r\n";
  process.writeData(cmd.c_str(), cmd.length());
  cmd = "exit\r\n";
  process.writeData(cmd.c_str(), cmd.length());

  process.waitForFinished();
  LOGD << process.getExitCode();
  char buf[1024]{0};
  process.readData(buf, 1024);

#else

  Process process("sh");
  process.setAsyncRead(false);
  process.start();

  string cmd = "ls -al\n";
  process.writeData(cmd.c_str(), cmd.length());
  cmd = "exit\n";
  process.writeData(cmd.c_str(), cmd.length());

  process.waitForFinished();
  LOGD << process.getExitCode();
  char buf[1024]{0};
  process.readData(buf, 1024);

#endif

  LOGD << "buf: " << buf;
}
