/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/File.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <iostream>
#include <vector>

using namespace std;
USE_NS_FF

TEST(FileTest, FileList){
	File file("/");

	auto iter = file.iterator();

	while(iter.next()){
		cout << iter.getFile() << endl;
	}
}

TEST(FileTest, FileRead){
	system("echo 123 >> test.txt");
	system("echo 456 >> test.txt");
	system("echo 7890 >> test.txt");
	system("echo end >> test.txt");
	system("cat test.txt");

	cout << "==========================" << endl;

	File file("test.txt");
	cout << file.readAll().toString() << endl;
	file.remove();
}
