/*
 * String_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */


#include <gtest/gtest.h>
#include <ff/Serial.h>
#include "TestDef.h"
#include <iostream>
#include <algorithm>
#include <thread>

using namespace std;
USE_NS_FF

TEST(SerialTest, SerialTest){
	Serial serial;
	serial.open("COM8");
	thread rd([&]{
		while(true){
			char buf[32] = {0};
			if(-1 == serial.read(buf, 31, 500)) break;
			LOGD << "buf: " << buf;
		}
	});
	thread wr([&]{
		while(true){
			if(-1 == serial.send("1234567890", 10)) break;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	});

	thread([&]{
		this_thread::sleep_for(chrono::milliseconds(10 * 1000));
		serial.close();
	}).detach();

	rd.join();
	wr.join();
}
