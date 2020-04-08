/*
 * Base64_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */


#include <gtest/gtest.h>
#include <ff/Base64.h>
#include <ff/Tick.h>
#include "TestDef.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace NS_FF;

TEST(Base64Test, Base64Test){
	string str = "test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64"
			"test string for base64";

	int i = 0;
	Tick tick;
	tick.tick();
	while (++i < 1000) {
		auto strEnc = Base64::Encrypt(str);
		//cout << strEnc << endl;
		stringstream ostr;
		Base64::Decrypt(ostr, strEnc);
		//cout << ostr.str() << endl;
		EXPECT_EQ(str, ostr.str());
	}
	cout << "tick: " << tick.tock() << endl;
}
