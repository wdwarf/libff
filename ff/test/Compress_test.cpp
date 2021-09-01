/*
 * Compress_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/Zip.h>
#include <ff/Unzip.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <fstream>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(CompressTest, CompressTest) {
  system("rm -rf compress_test");

  system("mkdir compress_test");
  system("mkdir compress_test/sub_dir");
  system("mkdir compress_test/sub_dir_c");
  system("mkdir compress_test/sub_dir/sub_dir2");
  system("echo \"qwertyuiop[]asdfghjkl;'zxcvbnm,./1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./1234567890-=asdfasdfasfaasdqwertyuiodfasdf12341234!@#$!#%#$%^$#%^&p[]asdfghjkl;'zxcasdfasvbnm,./1234567890-=dfasdfasd\" > compress_test/sub_dir/hello.txt");
  system("echo hello > compress_test/sub_dir/sub_dir2/hello.txt");
  system("echo hello > compress_test/sub_dir/hello2.txt");
  system("echo hello > compress_test/hello1.txt");
  system("echo hello > compress_test/sub_dir_c/hello1.txt");

  Zip zip("compress_test.zip");
  zip.createNew();
  zip.zip("compress_test");
  zip.close();

  Unzip unzip("compress_test.zip");
  auto files = unzip.list();
  for(auto& file : files){
    cout << file.name 
      << ", " << file.uncompressedSize
      << ", " << file.compressedSize
      << ", %" << file.ratio
      << ", " << file.method
      << ", " << (file.date.year) << "-" << (file.date.mon + 1) << "-" << file.date.mday
      << " " << file.date.hour << ":" << file.date.min << ":" << file.date.sec
      << endl;
  }

  system("rm -rf compress_test");
  unzip.unzipTo({"compress_test/hello1.txt", "compress_test/sub_dir/hello.txt"},"test_dir", true);
  // system("rm -rf test_dir");
  system("rm -rf compress_test.zip");
}
