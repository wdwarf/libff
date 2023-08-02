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

  stringstream stream;
  stream << endl;
  char buf[10240]{0};
  int readBytes = 0;
  while ((readBytes = process.readData(buf, 10240)) > 0) {
    stream.write(buf, readBytes);
    memset(buf, 0, sizeof(buf));
  }
  LOGD << stream.str();

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

  stringstream stream;
  stream << endl;
  char buf[10240]{0};
  int readBytes = 0;
  while ((readBytes = process.readData(buf, 10240)) > 0) {
    stream.write(buf, readBytes);
    memset(buf, 0, sizeof(buf));
  }
  LOGD << stream.str();

#endif
}
