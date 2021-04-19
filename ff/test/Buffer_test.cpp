/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Buffer.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <iostream>
#include <vector>

using namespace std;
USE_NS_FF

TEST(BufferTest, BufferTest){
	Buffer buf, buf2;
	buf << '1';
	buf2 << buf;
	LOGD << buf.toHexString();
	LOGD << (buf == buf2);
	buf << buf;
	LOGD << buf.toHexString();
	buf << 0x12345678;
	LOGD << buf.toHexString();
	buf.reverse();
	LOGD << buf.toHexString();

	string s = "12 34 56780x90\n 0xff 0xab cd";
	LOGD << s;
	Buffer buf3;
	buf3.fromHexString(s);
	LOGD << buf3.toHexString();

}
