/*
 * Curl_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/FileSystem.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(FileSystemTest, FileSystemTest) {
#ifdef _WIN32
  string path = "C:";
#else
  string path = "/";
#endif
  FileSystem fs(path);
  auto si = fs.df();
  cout << "total: " << si.total << endl
    << "free: " << si.free << endl
     << "available: " << si.available << endl
      << "used: " << si.used << endl;
}
