/**
 * @file NamedPipe_test.cpp
 * @author DuckyLi
 * @date 2023-08-12 10:06:55
 * @description
 */

#include <ff/NamedPipe.h>
#include <gtest/gtest.h>

#include <iostream>
#include <thread>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(NamedPipeTest, NamedPipeTest) {
  LOGD << "===========";
  NamedPipe server;
  NamedPipe client;

#ifdef _WIN32
  const std::string pipeName = "\\\\.\\pipe\\pipe_ff";
#else
  const std::string pipeName = "/mnt/ff_test_named_pipe";
#endif

  ASSERT_TRUE(server.create(pipeName));
  ASSERT_TRUE(client.open(pipeName));
  ASSERT_TRUE(server.connect());
  
  LOGD << "client write: " << client.write("hello", 5);
  char buf[10]{0};
  LOGD << "server read: " << server.read(buf, 9);
  LOGD << buf;
  ASSERT_EQ(server.read(buf, 9), -1);

  
  LOGD << "server write: " << server.write("hello", 5);
  memset(buf, 0, sizeof(buf));
  LOGD << "client read: " << client.read(buf, 9);
  LOGD << buf;
  ASSERT_EQ(client.read(buf, 9), -1);
}
