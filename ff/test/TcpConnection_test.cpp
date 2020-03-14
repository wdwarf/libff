/*
 * TcpServer_test.cpp
 *
 *  Created on: Jan 17, 2020
 *      Author: u16
 */

#include <gtest/gtest.h>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <ff/Buffer.h>
#include <ff/String.h>
#include <ff/TcpConnection.h>
#include "TestDef.h"

using namespace std;
using namespace std::chrono;
using namespace NS_FF;

TEST(TcpConnectionTest, TcpConnectionTest4) {
	bool stoped = false;
	bool exitFlag = false;

	bool svrAccepted = false;
	bool dataRecved = false;

	mutex m;
	condition_variable cond;
	std::unique_lock<mutex> lk(m);

	TcpConnectionPtr svr = TcpConnection::CreateInstance();
	TcpConnectionPtr clientPtr = TcpConnection::CreateInstance();

	EXPECT_TRUE(svr->listen(5678, "127.0.0.1", IpVersion::V4)) << "server listen failed.";
	set<TcpConnectionPtr> clients;
	svr->onAccept([&](const TcpConnectionPtr& tcpSock) {
		svrAccepted = true;
		clients.insert(tcpSock);
		LOGD << "tcp client connected." << tcpSock->getSocket().getRemoteAddress()
				<< ":" << tcpSock->getSocket().getRemotePort();
		//client = tcpSock;
			tcpSock->onData([&exitFlag, &svr](const uint8_t* data, uint32_t len, const TcpConnectionPtr& conn) {
						LOGD << "data: " << Buffer::ToHexString(data, len) << endl;
						if("exit" == String((const char*)data, len).trim()) {
							conn->close();
							exitFlag = true;
							return;
						}
						conn->send(data, len);
					});
			tcpSock->onClose([&exitFlag, &svr, &clients](const TcpConnectionPtr& conn) {
						LOGD << "tcp client closed";
						//svr.stop();
						clients.erase(conn);
						cout << clients.size() << endl;
						if(exitFlag && clients.empty()) svr->close();
						//cout << "tcpSock.use_count(): " << tcpSock.use_count() << endl;
					});

		});
	svr->onClose([&](const TcpConnectionPtr& connection) {
		LOGD << "tcp server stoped";
		std::unique_lock<mutex> lk(m);
		stoped = true;
		cond.notify_one();
	});

	EXPECT_TRUE(clientPtr->connect(5678, "127.0.0.1", 5679, "127.0.0.1"));
	clientPtr->onData(
			[&clientPtr, &dataRecved](const uint8_t* data, uint32_t len, const TcpConnectionPtr& conn) {
				dataRecved = true;
				LOGD << "server rsp data: " << Buffer::ToHexString(data, len) << endl;
				clientPtr->send("exit", 4);
			});
	clientPtr->onClose([](const TcpConnectionPtr& connection) {
		LOGD << "disconnected.";
	});
	clientPtr->send("1234", 4);

	cond.wait_for(lk, seconds(5), [&] {return stoped;});

	EXPECT_TRUE(svrAccepted);
	EXPECT_TRUE(dataRecved);
}

TEST(TcpConnectionTest, TcpConnectionTest6) {
	bool stoped = false;
	bool exitFlag = false;
	mutex m;
	condition_variable cond;
	std::unique_lock<mutex> lk(m);

	TcpConnectionPtr svr = TcpConnection::CreateInstance();
	TcpConnectionPtr clientPtr = TcpConnection::CreateInstance();

	if(!svr->listen(5678, "fe80::aced:719b:c05d:5f5%ens33", IpVersion::V6)){
		LOGD << "server listen failed.";
		return;
	}
	set<TcpConnectionPtr> clients;
	svr->onAccept([&](const TcpConnectionPtr& tcpSock) {
		clients.insert(tcpSock);
		LOGD << "tcp client connected." << tcpSock->getSocket().getRemoteAddress()
				<< ":" << tcpSock->getSocket().getRemotePort();
		//client = tcpSock;
			tcpSock->onData([&exitFlag, &svr](const uint8_t* data, uint32_t len, const TcpConnectionPtr& conn) {
						LOGD << "data: " << Buffer::ToHexString(data, len) << endl;
						if("exit" == String((const char*)data, len).trim()) {
							conn->close();
							exitFlag = true;
							return;
						}
						conn->send(data, len);
					});
			tcpSock->onClose([&exitFlag, &svr, &clients](const TcpConnectionPtr& conn) {
						LOGD << "tcp client closed";
						clients.erase(conn);
						if(exitFlag && clients.empty()) svr->close();
					});

		});
	svr->onClose([&](const TcpConnectionPtr& connection) {
		LOGD << "tcp server stoped";
		std::unique_lock<mutex> lk(m);
		stoped = true;
		cond.notify_one();
	});

	clientPtr->connect(5678, "fe80::aced:719b:c05d:5f5%ens33", 5679, "fe80::aced:719b:c05d:5f5%ens33");
	clientPtr->onData(
			[&clientPtr](const uint8_t* data, uint32_t len, const TcpConnectionPtr& conn) {
				LOGD << "server rsp data: " << Buffer::ToHexString(data, len) << endl;
				clientPtr->send("exit", 4);
			});
	clientPtr->onClose([](const TcpConnectionPtr& connection) {
		LOGD << "disconnected.";
	});
	clientPtr->send("1234", 4);

	cond.wait_for(lk, seconds(5), [&] {return stoped;});
}

