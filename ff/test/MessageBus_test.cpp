/*
 * MessageBus_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Buffer.h>
#include <ff/MessageBus.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(MessageBusTest, MessageBusTest) {
  MessageBusServer svr;
  svr.start(6600);

  MessageBusClient client1(1);
  client1.on(101, [&client1](const MsgBusPackage& pkg) {
		auto remoteHdr = pkg.header();
    MsgBusPkgHeader hdr = *remoteHdr;

		cout << "client1 got msg 101" << endl;

    // hdr.code(MsgCode::MsgTrans);
    hdr.from(client1.clientId());
    hdr.target(remoteHdr->from());
    hdr.options(MsgOpt::Rsp);

    MsgBusPackage rspPkg;
    rspPkg.generate(hdr);
    client1.send(rspPkg);
  });
  client1.start(6600, "127.0.0.1", 0);

  MessageBusClient client2(2);
  client2.on(101, [&client2](const MsgBusPackage& pkg) {
		auto remoteHdr = pkg.header();
    MsgBusPkgHeader hdr = *remoteHdr;

		cout << "client2 got msg 101" << endl;

    // hdr.code(MsgCode::MsgTrans);
    hdr.from(client2.clientId());
    hdr.target(remoteHdr->from());
    hdr.options(MsgOpt::Rsp);

    MsgBusPackage rspPkg;
    rspPkg.generate(hdr);
    client2.send(rspPkg);
  });
  client2.start(6600, "127.0.0.1", 0);

  MessageBusClient client3(3);
  client3.on(102, [](const MsgBusPackage& pkg) {});
  client3.on(1000, [](const MsgBusPackage& pkg) {});
  client3.start(6600, "127.0.0.1", 0);
  client3.on(2000, [](const MsgBusPackage& pkg) {});

  thread([&client3, &client2, &client1]() {
    do {
      this_thread::sleep_for(chrono::milliseconds(100));
      cout << "start req" << endl;

      auto promise = client3.req(101, 1);
      auto pkg = promise->get(5 * 1000);
      if (pkg.get()) {
        cout << "responsed: " << pkg->header()->msgId() << endl;
      } else {
        cout << "response timeout" << endl;
      }

      promise = client3.req(101, 0);
      pkg = promise->get(5 * 1000);
      if (pkg.get()) {
        cout << "responsed: " << pkg->header()->msgId() << endl;
      } else {
        cout << "response timeout" << endl;
      }
    } while (false);
  }).join();

  // while (true) {
    this_thread::sleep_for(chrono::seconds(5));
  // }
	client1.stop();
	client2.stop();
  client3.stop();
	svr.stop();
	this_thread::sleep_for(chrono::seconds(1));
}
