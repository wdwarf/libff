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
	cout << buf.toString() << endl;
	cout << (buf == buf2) << endl;
	buf << buf;
	cout << buf.toString() << endl;
	buf << 0x12345678;
	cout << buf.toString() << endl;
	buf.reverse();
	cout << buf.toString() << endl;
}
