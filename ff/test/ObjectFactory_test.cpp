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
USE_NS_FF

TEST(TestObjectFactory, TestObjectFactory){
	ObjectFactory o;
	o.regiesterCreator<string>("str", ObjectType::Singleton);
	EXPECT_TRUE(o.hasCreator("str"));
	EXPECT_FALSE(o.hasCreator("strxxx"));

	auto str = o.createObject<string>("str");
	*str = "this is a test text";

	LOGD << *str;

	auto str2 = o.createObject<string>("str");

	EXPECT_EQ(*str, *str2);

	*o.createObject<string>("str") = "new string";
	LOGD << *str;

	try{
		o.createObject<string>("class not exists");
	}catch(std::exception& e){
		LOGD << e.what();
	}
}

TEST(TestObjectFactory, TestParamCreate){
	ObjectFactory o;
	o.regiesterCreator<string>("str", ObjectType::Singleton, "this is a test string");
	o.regiesterCreator<string>("str2", ObjectType::New);

	auto s = o.createObject<string>("str");
	cout << *s << endl;
	s->assign("lalala~!");
	cout << *s << endl;

	s = o.createObject<string>("str2", "hahaha~!");
	cout << *s << endl;
	s = o.createObject<string>("str2", "hehehe~!");
	cout << *s << endl;

	s = o.createObject<string>("str", "123");
	cout << *s << endl;
	s = o.createObject<string>("str", "456");
	cout << *s << endl;
	
	o.unregiesterCreator("str");
	try{
		s = o.createObject<string>("str", "456");
	}catch(std::exception& e){
		LOGD << e.what();
	}
}
