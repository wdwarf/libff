/*
 * TcpClient_test.cpp
 *
 *  Created on: Aug 12, 2019
 *      Author: root
 */

#include <gtest/gtest.h>
#include <iostream>
#include <ff/TcpClient.h>

using namespace std;
using namespace NS_FF;

class MyTcpClient : public TcpClient{
	void onStartFailed(const std::string& errInfo){
		cout << errInfo << endl;
		this->stop();
	}

	void onStop(){
		cout << "stoped" << endl;
	}
};

TEST(TestTcpClient, TestTcpClient){
	MyTcpClient tcpClient;
	tcpClient.start();
	sleep(3);
	cout << "end " << endl;
}
