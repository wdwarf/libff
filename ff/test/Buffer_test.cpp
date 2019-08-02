/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Buffer.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
using namespace NS_FF;

TEST(BufferTest, BufferTest){
	Buffer buf, buf2;
	buf << '1';
	buf2 << buf;
	cout << buf << endl;
	cout << (buf == buf2) << endl;
	buf << buf;
	cout << buf << endl;
}
