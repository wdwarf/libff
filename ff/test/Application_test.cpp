/*
 * String_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Application.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(ApplicationTest, ApplicationTest) {
  LOGD << "env $PATH: " << Application::GetEnv("PATH");

  LOGD << "PutEnv: " << Application::SetEnv("test_env", "test_env_value");
  LOGD << "env $test_env: " << Application::GetEnv("test_env");
  EXPECT_EQ("test_env_value", Application::GetEnv("test_env"));
  Application::UnsetEnv("test_env");
  LOGD << "env $test_env: " << Application::GetEnv("test_env");
}
