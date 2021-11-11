/*
 * Socket_test.cpp
 *
 *  Created on: Aug 9, 2019
 *      Author: ducky
 */

#include <ff/Socket.h>
#include <gtest/gtest.h>

#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

#define SVR_PORT 65008

TEST(TestSocket, TestSocket) {
  Socket svrSock;
  svrSock.createTcp();
  EXPECT_NE(svrSock.getHandle(), INVALID_SOCKET);
  EXPECT_TRUE(svrSock.bind(SVR_PORT));
  EXPECT_TRUE(svrSock.listen());

  thread acceptThread([&] {
    sockaddr_in addr;
    Socket clientSock = svrSock.accept(addr);
    EXPECT_NE(clientSock.getHandle(), INVALID_SOCKET);
    if (INVALID_SOCKET == clientSock.getHandle()) {
      LOGE << "accept failed";
      return;
    }
    LOGD << "accept [" << clientSock.getHandle() << "]";
    clientSock.send("welcom !", 8);
  });

  this_thread::sleep_for(chrono::milliseconds(100));

  thread clientThread([&] {
    Socket sock;
    sock.createTcp();
    EXPECT_TRUE(sock.connect("127.0.0.1", SVR_PORT));

    LOGD << "client connected";
    char buf[1024] = {0};
    while (sock.read(buf, 1024, 1000) > 0) {
      LOGD << buf;
    }
    LOGD << "client disconnected";

    sock.close();
    svrSock.close();
  });

  acceptThread.join();
  clientThread.join();
}
