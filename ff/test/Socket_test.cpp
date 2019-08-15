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
using namespace NS_FF;

TEST(TestSocket, TestSocket) {
	Socket sock;
	sock.createTcp();
	EXPECT_GT(sock.getHandle(), 0);

	thread acceptThread([] {
		Socket sock;
		sock.createTcp();
		sock.bind(65001);
		sock.listen();
		sockaddr_in addr;
		Socket clientSock = sock.accept(addr);
		if(clientSock.getHandle() > 0) {
			LDBG << "accept [" << clientSock.getHandle() << "]";
			clientSock.send("welcom !", 8);
		}
		clientSock.close();
		sock.close();
	});

	thread clientThread([] {
		Socket sock;
		sock.createTcp();
		if(sock.connect("127.0.0.1", 65001)) {
			LDBG << "client connected";
			char buf[1024] = {0};
			while(sock.read(buf, 1024, 1000) > 0)
			{
				LDBG << buf;
			}
			LDBG << "client disconnected";
		}

		sock.close();
	});

	acceptThread.join();
	clientThread.join();
}
