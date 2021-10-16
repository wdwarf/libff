/*
 * Compress_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/File.h>
#include <ff/Unzip.h>
#include <ff/Zip.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(CompressTest, CompressTest) {
  File("compress_test").remove(true);
  File("compress_test.zip").remove();

  system("mkdir compress_test");
  system(("mkdir " + File("compress_test/sub_dir").getPath()).c_str());
  system(("mkdir " + File("compress_test/sub_dir_c").getPath()).c_str());
  system(("mkdir " + File("compress_test/sub_dir/sub_dir2").getPath()).c_str());
  system(
      "echo "
      "\"qwertyuiop[]asdfghjkl;'zxcvbnm,./"
      "1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./"
      "1234567890-=asdfasdfasfaasdqwertyuiodfasdf12341234!@#$!#%#$%^$#%^&p[]"
      "asdfghjkl;'zxcasdfasvbnm,./1234567890-=dfasdfasd\" > "
      "compress_test/sub_dir/hello.txt");
  system("echo hello > compress_test/sub_dir/sub_dir2/hello.txt");
  system("echo hello > compress_test/sub_dir/hello2.txt");
  system("echo hello > compress_test/hello1.txt");
  system("echo hello > compress_test/sub_dir_c/hello1.txt");

  Zip zip("compress_test.zip");
  zip.createNew();
  zip.zip("compress_test");
  zip << ZipEntry("test/bb.txt");
  zip << "compress_test/sub_dir/hello2.txt";
  zip << ZipEntry("lala.txt");
  zip << "compress_test/sub_dir/hello2.txt";
  zip.write("1234567890", 10);
  zip << ZipEntry("bb2.txt");
  zip << "compress_test/sub_dir/hello2.txt";
  zip.close();

  Unzip unzip("compress_test.zip");
  auto files = unzip.list();
  for (auto& file : files) {
    cout << file.name << ", " << file.uncompressedSize << ", "
         << file.compressedSize << ", %" << file.ratio << ", " << file.method
         << ", " << (file.date.year) << "-" << (file.date.mon + 1) << "-"
         << file.date.mday << " " << file.date.hour << ":" << file.date.min
         << ":" << file.date.sec << endl;
  }

  File("compress_test").remove(true);
  unzip.unzipTo({"compress_test/hello1.txt", "compress_test/sub_dir/hello.txt"},
                "test_dir", true);
  File("compress_test.zip").remove();
}
