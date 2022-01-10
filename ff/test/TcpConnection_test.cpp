/*
 * TcpServer_test.cpp
 *
 *  Created on: Jan 17, 2020
 *      Author: liyawu
 */

#include <ff/Buffer.h>
#include <ff/String.h>
#include <ff/TcpConnection.h>
#include <ff/Thread.h>
#include <ff/Tick.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <set>
#include <thread>

#include "TestDef.h"

using namespace std;
using namespace std::chrono;
USE_NS_FF

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

  EXPECT_TRUE(svr->listen(5678, "127.0.0.1", AF_INET))
      << "server listen failed.";
  set<TcpConnectionPtr> clients;
  svr->onAccept([&](const TcpConnectionPtr& tcpSock) {
    svrAccepted = true;
    clients.insert(tcpSock);
    LOGD << "tcp client connected." << tcpSock->getSocket().getRemoteAddress()
         << ":" << tcpSock->getSocket().getRemotePort();
    // client = tcpSock;
    tcpSock->onData([&exitFlag, &svr](const uint8_t* data, uint32_t len,
                                      const TcpConnectionPtr& conn) {
      LOGD << "data: " << Buffer::ToHexString(data, len);
      if ("exit" == String((const char*)data, len).trim()) {
        conn->close();
        exitFlag = true;
        return;
      }
      conn->send(data, len);
    });
    tcpSock->onClose([&exitFlag, &svr, &clients](const TcpConnectionPtr& conn) {
      LOGD << "tcp client closed";
      // svr.stop();
      clients.erase(conn);
      LOGD << clients.size();
      if (exitFlag && clients.empty()) svr->close();
      // cout << "tcpSock.use_count(): " << tcpSock.use_count() << endl;
    });
  });
  svr->onClose([&](const TcpConnectionPtr& connection) {
    LOGD << "tcp server stoped";
    std::unique_lock<mutex> lk(m);
    stoped = true;
    cond.notify_one();
  });

  EXPECT_TRUE(clientPtr->connect(5678, "127.0.0.1", 5679, "127.0.0.1"));
  clientPtr->onData([&clientPtr, &dataRecved](const uint8_t* data, uint32_t len,
                                              const TcpConnectionPtr& conn) {
    dataRecved = true;
    LOGD << "server rsp data: " << Buffer::ToHexString(data, len);
    // clientPtr->send("exit", 4);
  });
  clientPtr->onClose(
      [](const TcpConnectionPtr& connection) { LOGD << "disconnected."; });
  clientPtr->send("1234", 4);
  Thread::Sleep(100);
  clientPtr->send("5678", 4);
  Thread::Sleep(100);
  clientPtr->send("90ab", 4);
  Thread::Sleep(1000);
  clientPtr->send("exit", 4);

  cond.wait_for(lk, seconds(5), [&] { return stoped; });

  EXPECT_TRUE(svrAccepted);
  EXPECT_TRUE(dataRecved);
}

const string constText =
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow."
    "Computers are some of the most versatile tools that we have available. "
    "They are capable of performing stunning feats of computation, they allow "
    "information to be exchanged easily regardless of their physical location, "
    "they simplify many every-day tasks, and they allow us to automate many "
    "processes that would be tedious or boring to perform otherwise. However, "
    "computers are not \"intelligent\" as we are. They have to be told in no "
    "uncertain terms exactly what they're supposed to do, and their native "
    "languages are quite unlike anything we speak. Thus, there's a formidable "
    "language barrier between a person who wishes a computer to do something, "
    "and the computer that typically requires instructions in its native "
    "language, machine code, to do anything. So far, computers cannot figure "
    "out what they are supposed to do on their own, and thus they rely on "
    "programs which we create, which are sets of instructions that the "
    "computer can understand and follow.";

TEST(TcpConnectionTest, TcpConnectionTest) {
  TcpConnectionPtr svr = TcpConnection::CreateInstance();

  set<TcpConnectionPtr> clients;
  mutex m;

  list<BufferPtr> buffers;
  atomic_int acceptCnt(0);

  svr->onAccept([&](const TcpConnectionPtr& tcpSock) {
    if (!tcpSock) return;
    lock_guard<mutex> lk(m);
    ++acceptCnt;
    clients.insert(tcpSock);
    BufferPtr buf(new Buffer());
    buf->setCapacity(3 * 1024 * 1024);

#if 1
    tcpSock->onClose(
        [&m, &svr, &clients, &buffers, buf](const TcpConnectionPtr& conn) {
          // LOGD << "tcp client closed";
          // svr.stop();
          lock_guard<mutex> lk(m);
          buffers.push_back(buf);
          LOGD << "clientsNow: " << clients.size()
               << ", buffers: " << buffers.size();
          clients.erase(conn);
        });
#endif

#if 1

    // LOGD << "tcp client connected. " <<
    // tcpSock->getSocket().getRemoteAddress()
    //      << ":" << tcpSock->getSocket().getRemotePort();
    // client = tcpSock;

    tcpSock->onData([&svr, buf](const uint8_t* data, uint32_t len,
                                const TcpConnectionPtr& conn) {
      try {
        buf->append(data, len);
      } catch (std::exception& e) {
        LOGE << "err: " << e.what();
      }
      // this_thread::sleep_for(chrono::milliseconds(5));
    });
#endif
  });
  svr->onClose(
      [&](const TcpConnectionPtr& connection) { LOGD << "tcp server stoped"; });

  EXPECT_TRUE(svr->listen(5678, "", AF_INET)) << "server listen failed.";

  do {
    buffers.clear();
    acceptCnt = 0;
    const int cnt = 50;
    const uint32_t clientCnt = 110;
    uint32_t currentClientCnt = clientCnt;
    for (int i = 0; i < clientCnt; ++i) {
      thread([&m, &currentClientCnt, cnt]() {
        do {
          Socket sock;
          sock.createTcp();
          sock.setBlocking(SockBlockingType::Blocking);
          sock.setUseSelect(false);

          if (!sock.connect("192.168.2.53", 5678, 10000)) {
            LOGE << "connect to server failed";
            continue;
          }

          Tick t;
          int sendBytes = 0;

          for (int i = 0; i < cnt; ++i) {
            sendBytes += sock.send(constText.c_str(), constText.length());
          }
          LOGD << "tick: " << t.tock() << ", sendBytes: " << sendBytes
               << ", total: " << (constText.length() * cnt);

          sock.close();
          lock_guard<mutex> lk(m);
          --currentClientCnt;
          LOGD << "client closed";
          break;
        } while (true);
      }).detach();
    }

    Tick t;
    while (buffers.size() != clientCnt && t.tock() < 300 * 1000) {
      LOGD << "clients: " << clients.size() << ", buffers: " << buffers.size()
           << ", acceptCnt: " << acceptCnt.load()
           << ", currentClientCnt: " << currentClientCnt;
      this_thread::sleep_for(chrono::milliseconds(1000));
    }

    LOGD << "clients: " << clients.size() << ", buffers: " << buffers.size();

    for (auto buf : buffers) {
      if (buf->getSize() != (constText.length() * cnt)) {
        LOGE << "invalid size: " << buf->getSize() << ", "
             << (constText.length() * cnt);
      }
    }

    LOGD << "acceptCnt: " << acceptCnt.load();

    LOGD << "end....";

    if (buffers.size() != clientCnt || !clients.empty()) {
      LOGD << "clientsEnd: " << clients.size()
           << ", buffers: " << buffers.size();

      break;
    }

    this_thread::sleep_for(chrono::seconds(3));
  } while (true);

  svr->close();
}

TEST(TcpConnectionTest, TcpConnectionTest6) {
  bool stoped = false;
  bool exitFlag = false;
  mutex m;
  condition_variable cond;
  std::unique_lock<mutex> lk(m);

  TcpConnectionPtr svr = TcpConnection::CreateInstance();
  TcpConnectionPtr clientPtr = TcpConnection::CreateInstance();

  EXPECT_TRUE(
      svr->listen(5678, "fe80::aced:719b:c05d:5f5%ens33", AF_INET6))
      << "server listen failed.";

  set<TcpConnectionPtr> clients;
  svr->onAccept([&](const TcpConnectionPtr& tcpSock) {
    clients.insert(tcpSock);
    LOGD << "tcp client connected." << tcpSock->getSocket().getRemoteAddress()
         << ":" << tcpSock->getSocket().getRemotePort();
    // client = tcpSock;
    tcpSock->onData([&exitFlag, &svr](const uint8_t* data, uint32_t len,
                                      const TcpConnectionPtr& conn) {
      LOGD << "data: " << Buffer::ToHexString(data, len);
      if ("exit" == String((const char*)data, len).trim()) {
        conn->close();
        exitFlag = true;
        return;
      }
      conn->send(data, len);
    });
    tcpSock->onClose([&exitFlag, &svr, &clients](const TcpConnectionPtr& conn) {
      LOGD << "tcp client closed";
      clients.erase(conn);
      if (exitFlag && clients.empty()) svr->close();
    });
  });
  svr->onClose([&](const TcpConnectionPtr& connection) {
    LOGD << "tcp server stoped";
    std::unique_lock<mutex> lk(m);
    stoped = true;
    cond.notify_one();
  });

  clientPtr->connect(5678, "fe80::aced:719b:c05d:5f5%ens33", 5679,
                     "fe80::aced:719b:c05d:5f5%ens33");
  clientPtr->onData([&clientPtr](const uint8_t* data, uint32_t len,
                                 const TcpConnectionPtr& conn) {
    LOGD << "server rsp data: " << Buffer::ToHexString(data, len);
    clientPtr->send("exit", 4);
  });
  clientPtr->onClose(
      [](const TcpConnectionPtr& connection) { LOGD << "disconnected."; });
  clientPtr->send("1234", 4);

  cond.wait_for(lk, seconds(5), [&] { return stoped; });
}
