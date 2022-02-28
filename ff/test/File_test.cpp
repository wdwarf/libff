/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/File.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(FileTest, FileList) {
  File file("/");

  auto iter = file.iterator();

  while (iter.next()) {
    cout << iter.getFile() << endl;
  }
}

TEST(FileTest, FileRead) {
	{
		File f("\\\\Project\\mytest");
		cout << "path: " << f.getPath() << endl;
		
		f = File("/Project/mytest");
		cout << "path2: " << f.getPath() << endl;

	}
  system("echo 123 >> test.txt");
  system("echo 456 >> test.txt");
  system("echo 7890 >> test.txt");
  system("echo end >> test.txt");
        system(
#ifdef _WIN32
		"type"
#else
		"cat"
#endif
		" test.txt"
		);

	cout << "==========================" << endl;

	File file("test.txt");
	cout << file.readAll().toString() << endl;
	file.remove();
}
