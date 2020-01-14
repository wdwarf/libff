/*
 * TcpClient_test.cpp
 *
 *  Created on: Aug 12, 2019
 *      Author: root
 */

#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <ff/TcpClient.h>
#include <ff/Socket.h>
#include "TestDef.h"

using namespace std;
using namespace NS_FF;

namespace {
mutex m;
condition_variable cond;
bool dataArrived = false;
}

class MyTcpClient: public TcpClient {
	virtual void onConnected() override {
		LOGD << __func__;
		this->send("Hello !", 7);
	}

	virtual void onDisconnected() override {
		LOGD << __func__;
	}

	void onRecv(const uint8_t* buf, int bufLen) override {
		LOGD << Buffer::ToHexString(buf, bufLen);
		unique_lock<mutex> lk(m);

		dataArrived = true;
		cond.notify_one();
	}
};

template<typename T, int N>
void s(T (&arr)[N])
{
	int size = sizeof(T) * N;
	arr[0] = 12;
	LOGD << "size: " << size;
}

template<typename T>
void s(T &v)
{
	v = 100;
	LOGD << "v: " << v;
}

TEST(TestTcpClient, TestTcpClient) {
	char buf[1024];
	s(buf);
	LOGD << (int)buf[0];
	int i = 0;
	s(i);
	LOGD << "i : " << i;

	Socket svrSocket;
	thread t;
	if (svrSocket.createTcp() && svrSocket.bind(5432) && svrSocket.listen()) {

		t = thread([&] {
			LOGD << "accept thread started";
			if(svrSocket.getHandle() <= 0) return;
			sockaddr_in addr;
			Socket sock = svrSocket.accept(addr);

			char buf[32] = { 0 };
			sock.read(buf, sizeof(buf));
			LOGD << buf;

			sock.send("123", 3);

			sock.close();
		});
	}

	sleep(1);
	MyTcpClient tcpClient;
	tcpClient.setRemoteAddr("127.0.0.1");
	tcpClient.setRemotePort(5432);
	tcpClient.start();
	{
		unique_lock<mutex> lk(m);
		cond.wait_for(lk, std::chrono::seconds(2),
				[]{ return dataArrived; });

	}
	svrSocket.shutdown();
	svrSocket.close();
	if (t.joinable())
		t.join();

	LOGD << "end ";
}
