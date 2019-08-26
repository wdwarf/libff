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

using namespace std;
using namespace NS_FF;

TEST(BufferTest, BufferTest){
	Buffer buf, buf2;
	buf << '1';
	buf2 << buf;
	LOGD << buf.toString();
	LOGD << (buf == buf2);
	buf << buf;
	LOGD << buf.toString();
	buf << 0x12345678;
	LOGD << buf.toString();
	buf.reverse();
	LOGD << buf.toString();
}
