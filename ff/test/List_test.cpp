/**
 * @file List_test.cpp
 * @author liyawu
 * @date 2025-01-17 16:18:25
 * @description
 */

#include <ff/List.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(ListTest, ListTest) {
  List<int> intList = List<int>() << 1 << 2 << 3 << 4;
  for (auto& i : intList) {
    LOGD << i;
  }

  List<int> intList2{1, 2, 3, 4, 5};
  intList2 += {6, 7, 8};
  for (auto& i : intList2) {
    LOGD << i;
  }

  List<double> dbList;
  dbList.push_back(3.14);
  dbList.push_back(3.15);
  dbList.push_back(3.16);
  for (auto& i : dbList) {
    LOGD << i;
  }

  List<std::string> strList;
  strList.push_front("str1");
  strList.push_front("str2");
  strList.push_front("str3");
  for (auto& i : strList) {
    LOGD << i;
  }
  strList.sort();
  for (auto& i : strList) {
    LOGD << i;
  }
  ASSERT_FALSE(strList == (List<std::string>{"str1", "str2"}));
  ASSERT_TRUE(strList == (List<std::string>{"str1", "str2", "str3"}));
  strList.sort(
      [](const std::string& s1, const std::string& s2) { return s2 < s1; });
  for (auto& i : strList) {
    LOGD << i;
  }
  for (auto it = strList.rbegin(); it != strList.rend(); ++it) {
    LOGD << "r: " << *it;
  }
  strList.insert(1, "inserted");
  strList.insert(strList.end(), "inserted2");
  for (auto& i : strList) {
    LOGD << i;
  }
  ASSERT_TRUE(strList == (List<std::string>{"str3", "inserted", "str2", "str1",
                                            "inserted2"}));

  LOGD << "==============";
  List<std::string> sl(std::move(strList));
  ASSERT_TRUE(strList.empty());
  ASSERT_TRUE(sl == (List<std::string>{"str3", "inserted", "str2", "str1",
                                       "inserted2"}));
  ASSERT_EQ(1, sl.indexOf("inserted"));
}