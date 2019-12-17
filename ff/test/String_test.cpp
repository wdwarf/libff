/*
 * String_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */


#include <gtest/gtest.h>
#include <ff/String.h>
#include "TestDef.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace NS_FF;

TEST(TestString, TestString){
	String s = "   test string...   ";
	cout << "s: " << s << endl;
	EXPECT_TRUE(s.equals("   test string...   "));
	EXPECT_TRUE(s.equals("   Test String...   ", true));

	String tsl = s.trimLeft();
	EXPECT_TRUE(tsl.equals("test string...   "));
	EXPECT_TRUE(tsl.equals("Test String...   ", true));

	String tsr = s.trimRight();
	EXPECT_TRUE(tsr.equals("   test string..."));
	EXPECT_TRUE(tsr.equals("   Test String...", true));

	String ts = s.trim();
	EXPECT_TRUE(ts.equals("test string..."));
	EXPECT_TRUE(ts.equals("Test String...", true));

	String upper = ts.toUpper();
	EXPECT_TRUE(upper.equals("TEST STRING..."));
	String lower = upper.toLower();
	EXPECT_TRUE(lower.equals("test string..."));

	String replacedStr = ts.replace("test", "New");
	EXPECT_TRUE(replacedStr.equals("New string..."));
	String replacedStr2 = replacedStr.replace("new", "test", true);
	EXPECT_TRUE(replacedStr2.equals("test string..."));

	String replacedStr3 = replacedStr2.replaceAll(".", "~~~");
	EXPECT_TRUE(replacedStr3.equals("test string~~~~~~~~~"));

	EXPECT_EQ(4, replacedStr3.indexOf(" "));

	auto v = replacedStr3.split(IsAnyOf(" "));
	vector<string> v2{"test", "string~~~~~~~~~"};
	EXPECT_TRUE(equal(v.begin(), v.end(), v2.begin()));
}
