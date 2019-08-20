/*
 * TcpClient_test.cpp
 *
 *  Created on: Aug 12, 2019
 *      Author: root
 */

#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <ff/TcpClient.h>
#include <ff/Socket.h>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

class MyTcpClient: public TcpClient {
	void onStartFailed(const std::string &errInfo) {
		LDBG << "error: " << errInfo;
		this->stop();
	}

	void onStop() {
		LDBG << "stoped";
	}

	void onRecv(const BufferPtr &buffer) {
		LDBG << buffer->toString();
	}
};

TEST(TestTcpClient, TestTcpClient) {
	Socket svrSocket;
	svrSocket.createTcp();
	svrSocket.bind(5432);
	svrSocket.listen();

	thread t([&] {
		sockaddr_in addr;
		Socket sock = svrSocket.accept(addr);
		sock.send("123", 3);
		sock.close();
	});

	sleep(1);
	MyTcpClient tcpClient;
	tcpClient.setRemoteAddr("127.0.0.1");
	tcpClient.setRemotePort(5432);
	tcpClient.start();
	sleep(3);
	svrSocket.close();
	t.join();
	LDBG << "end ";
}
