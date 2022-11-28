/*
 * ScopeGuard_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/ScopeGuard.h>
#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

static std::string g_str = "123";

static void set_g_str(){
     g_str = "456";
}

TEST(ScopeGuardTest, ScopeGuardTest) {
  int n = 0;
  {
    auto g = ScopeGuard([&n] { ++n; });
  }
  EXPECT_EQ(n, 1);
  LOGD << "n: " << n;
  
  {
    auto g = ScopeGuard(set_g_str);
  }
  LOGD << g_str;
  EXPECT_EQ(g_str, "456");
}
