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

TEST(FileTest, FileTest){
	File file("/");

	auto iter = file.iterator();

	while(iter.next()){
		cout << iter.getFile() << endl;
	}
}
