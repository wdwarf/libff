/**
 * @file NamedPipe_test.cpp
 * @author DuckyLi
 * @date 2023-08-12 10:06:55
 * @description
 */

#include <ff/NamedPipe.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(NamedPipeTest, NamedPipeTest) {
  NamedPipe server;
  NamedPipe client;

#ifdef _WIN32

#else
  const std::string pipeName = "/mnt/ff_test_named_pipe";
  ASSERT_TRUE(server.create(pipeName));
  ASSERT_TRUE(server.open(pipeName));

#endif
}
