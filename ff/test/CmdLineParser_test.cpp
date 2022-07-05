/**
 * @file CmdLineParser_test.cpp
 * @auth DuckyLi
 * @date 2022-07-04 11:34:29
 * @description 
 */

#include <ff/CmdLineParser.h>
#include <ff/Settings.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(CmdLineParserTest, CmdLineParserTest) {
  const char* argv[] = {
    "",
    "--count=10",
    "--help",
    "-n=DuckyLi",
    "d:\\image1.jpg",
    "image2.jpg"
  };

  CmdLineParser parser({{"help", "", "show usage info"},
                        {"count,c", "", "calc count"},
                        {"name,n", "", "user name"},
                        {"@path2", "", "input file path2"},
                        {"@path", "", "input file path"}});

  LOGD << parser.usage();
  parser.parse(sizeof(argv) / sizeof(const char*), argv);
  LOGD << "name: " << parser.get("name").toString();
  LOGD << "count: " << (int)parser.get("count");
  LOGD << "path: " << parser.get("path").toString();
  LOGD << "path2: " << parser.get("path2").toString();

  Settings store;
  CmdLineParser parser2({{"help", "", "show usage info"},
                         {"count,c", "", "calc count"},
                         {"name,n", "", "user name"},
                         {"@path2", "", "input file path2"},
                         {"@path", "", "input file path"}},
                        store);

  LOGD << parser2.usage();
  parser2.parse(sizeof(argv) / sizeof(const char*), argv);
  LOGD << "name: " << parser2.get("name").toString();
  LOGD << "count: " << (int)parser2.get("count");
  LOGD << "path: " << parser2.get("path").toString();
  LOGD << "path2: " << parser2.get("path2").toString();
  
  LOGD << "name: " << store.getValue("name").toString();
  LOGD << "count: " << (int)store.getValue("count");
  LOGD << "path: " << store.getValue("path").toString();
  LOGD << "path2: " << store.getValue("path2").toString();

  EXPECT_EQ(parser.get("name").toString(), store.getValue("name").toString());
}
