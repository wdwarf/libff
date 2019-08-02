/*
 * Buffer_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Exception.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
using namespace NS_FF;

TEST(ExceptionTest, ExceptionTest){
	try{
		THROW_EXCEPTION(Exception, "ExceptionTest info.", -1);
	}catch(Exception& e){
		cout << e << endl;
	}
}
