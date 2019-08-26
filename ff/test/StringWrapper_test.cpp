/*
 * StringWrapper_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */


#include <gtest/gtest.h>
#include <ff/StringWrapper.h>
#include "TestDef.h"
#include <iostream>

using namespace std;
using namespace NS_FF;

TEST(TestStringWrapper, TestStringWrapper){
	LOGD << SW("this")(" ")("is")(" ")("a text from StringWrapper").toString();
	EXPECT_EQ("this is a text from StringWrapper", SW("this")(" ")("is")(" ")("a text from StringWrapper").toString());

	SW sw;
	sw << "this" << " " << "is " << "another" << " " << "text from StringWrapper";
	LOGD << sw.toString();
	EXPECT_EQ("this is another text from StringWrapper", sw.toString());
}
