/*
 * Compress_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/Zip.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <fstream>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(CompressTest, CompressTest) {
  system("mkdir compress_test");
  system("mkdir compress_test/sub_dir");
  system("mkdir compress_test/sub_dir_c");
  system("mkdir compress_test/sub_dir/sub_dir2");
  system("echo hello > compress_test/sub_dir/hello.txt");
  system("echo hello > compress_test/sub_dir/sub_dir2/hello.txt");
  system("echo hello > compress_test/sub_dir/hello2.txt");
  system("echo hello > compress_test/hello1.txt");
  system("echo hello > compress_test/sub_dir_c/hello1.txt");

  Zip zip("compress_test.zip");
  zip.createNew();
  zip.zip("compress_test");

  system("rm -rf compress_test");
}
