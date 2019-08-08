/*
 * ObjectFactory_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/ObjectFactory.h>
#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

TEST(TestObjectFactory, TestObjectFactory){
	ObjectFactory o;
	o.regiesterCreator<string>("str", ObjectType::Singleton);
	EXPECT_TRUE(o.hasCreator("str"));
	EXPECT_FALSE(o.hasCreator("strxxx"));

	auto str = o.createObject<string>("str");
	*str = "this is a test text";

	LDBG << *str;

	auto str2 = o.createObject<string>("str");

	EXPECT_EQ(*str, *str2);

	*o.createObject<string>("str") = "new string";
	LDBG << *str;

	try{
		o.createObject<string>("class not exists");
	}catch(std::exception& e){
		LDBG << e.what();
	}
}
