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
	void onStartFailed(const std::string &errInfo) override {
		LDBG << "error: " << errInfo;
		this->stop();
	}

	virtual void onConnected() override{
		LDBG << __func__;
		Thread::Sleep(5000);
		this->stop();
	}
	virtual void onDisconnected() override{
		LDBG << __func__;
		Thread::Sleep(5000);
		this->start();
	}

	void onRecv(const BufferPtr &buffer) override{
		LDBG << buffer->toString();
	}
};

TEST(TestTcpClient, TestTcpClient) {
	Socket svrSocket;
	thread t;
	if (svrSocket.createTcp()
			&& svrSocket.bind(5432)
			&& svrSocket.listen()) {

		t = thread([&] {
			LDBG << "accept thread started";
			if(svrSocket.getHandle() <= 0) return;
			sockaddr_in addr;
			Socket sock = svrSocket.accept(addr);
			sock.send("123", 3);
			sock.close();
		});
	}

	sleep(1);
	MyTcpClient tcpClient;
	tcpClient.setRemoteAddr("127.0.0.1");
	tcpClient.setRemotePort(5432);
	tcpClient.start();
	sleep(2);
	tcpClient.stop();
	sleep(2);
	tcpClient.stop();
	sleep(2);

	svrSocket.shutdown();
	svrSocket.close();
	if(t.joinable())
		t.join();

	MyTcpClient client2;
	LDBG << "end ";
}
