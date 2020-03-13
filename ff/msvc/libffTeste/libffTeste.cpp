// libffTeste.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <ff/Object.h>
#include <ff/DateTime.h>
#include <ff/File.h>
#include <ff/Socket.h>
#include <cstring>
#include <errno.h>
#include <ff/Application.h>
#include <ff/Variant.h>

using namespace std;
using namespace ff;

namespace Test {
	class T : public Object {

	};
}

int main()
{
	Variant v = 10;
	cout << v << endl;
	cout << Application::GetApplicationPath() << endl;
	cout << Application::GetApplicationName() << endl;

	Socket svr;
	svr.createTcp(IpVersion::V6);
	cout << "bind: " << svr.bind(60002, "fe80::3893:8250:80fa:aa0f%3") << endl;
	svr.listen();

	Socket s;
	cout << "create: " << s.createTcp(IpVersion::V6) << endl;
	cout << "conn: " << s.connect("fe80::3893:8250:80fa:aa0f%3", 60002) << endl;
	cout << "ver: " << (int)s.getIpVersion() << endl;
	SockAddr addr;
	Socket c = svr.accept(addr);
	cout << "c ver: " << (int)c.getIpVersion() << endl;
	cout << "send: " << s.send("Hello World!", 12) << endl;

	char buf[100] = { 0 };
	cout << "read: " << c.read(buf, 99) << endl;
	cout << "err: " << GetLastError() << endl;
	cout << buf << endl;

	cout << "end" << endl;
	cin.get();
	s.close();
    return 0;
}

