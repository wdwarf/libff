/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Curl.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <iostream>
#include <vector>

using namespace std;
USE_NS_FF

TEST(CurlTest, CurlTest){
	Curl c;
	c.setUrl("http://www.baidu.com");
	c.perform();
	cout << c.getOutputStream().str() << endl;
}
