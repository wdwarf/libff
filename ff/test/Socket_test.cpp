/*
 * Socket_test.cpp
 *
 *  Created on: Aug 9, 2019
 *      Author: ducky
 */

#include <gtest/gtest.h>
#include <ff/Socket.h>
#include <thread>
#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(TestSocket, TestSocket) {
	Socket svrSock;
	svrSock.createTcp();
	EXPECT_GT(svrSock.getHandle(), 0);
	svrSock.bind(65001);
	svrSock.listen();

	thread acceptThread([&] {
		sockaddr_in addr;
		Socket clientSock = svrSock.accept(addr);
		if(clientSock.getHandle() > 0) {
			LOGD << "accept [" << clientSock.getHandle() << "]";
			clientSock.send("welcom !", 8);
		}
		clientSock.close();
	});

	thread clientThread([&] {
		Socket sock;
		sock.createTcp();
		if(sock.connect("127.0.0.1", 65001)) {
			LOGD << "client connected";
			char buf[1024] = {0};
			while(sock.read(buf, 1024, 1000) > 0)
			{
				LOGD << buf;
			}
			LOGD << "client disconnected";
		}

		sock.close();
		svrSock.close();
	});

	acceptThread.join();
	clientThread.join();
}
