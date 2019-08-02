/*
 * ErrNo_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */
#include <gtest/gtest.h>
#include <ff/ErrNo.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace NS_FF;

void f(){}

TEST(TestErrNo, TestErrNo){
	try{
		thread t(&f);
//		thread([](){
//					//SetLastError(123, "err info");
//						//cout << "last errno: " << GetLastErrNo() << endl;
//						//cout << "last err info: " << GetLastErrInfo() << endl;
//			});
	}catch(exception& e){
		cout << e.what() << endl;
	}
}
