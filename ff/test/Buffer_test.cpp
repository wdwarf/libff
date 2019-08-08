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
	LDBG << buf.toString();
	LDBG << (buf == buf2);
	buf << buf;
	LDBG << buf.toString();
	buf << 0x12345678;
	LDBG << buf.toString();
	buf.reverse();
	LDBG << buf.toString();
}
