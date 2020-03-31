// libffTeste.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <ff/Object.h>
#include <ff/DateTime.h>
#include <ff/File.h>
#include <ff/Socket.h>
#include <ff/Buffer.h>
#include <cstring>
#include <errno.h>
#include <ff/Application.h>
#include <ff/Variant.h>
#include <ff/Random.h>
#include <ff/Endian.h>
#include <ff/DllLoader.h>

using namespace std;
using namespace ff;

namespace Test {
	class T : public Object {

	};
}

struct SS {
	int n;
	int m;
	char buf[16];
};

int main()
{
	try {
		typedef int (*FntestDll)(void);
		FntestDll f = nullptr;
		DllLoader ld("D:\\Projects\\libff\\ff\\msvc\\libff\\Debug\\testDll.dll");
		f = (FntestDll)ld.getProc("fntestDll");
		cout << f() << endl;


	}
	catch (exception& e) {
		cout << e.what() << endl;
		cin.get();
		return -1;
	}


	float f = 0.01;
	cout << Buffer(&f, sizeof(f)).toHexString() << endl;
	cout << Buffer(&f, sizeof(f)).toBinaryString() << endl;
	int n = 0x01;
	cout << Buffer(&n, 4).toHexString() << endl;
	auto endian = Endian::GetHostEndian();
	n = endian.toBig(n);
	cout << Buffer(&n, 4).toHexString() << endl;
	int i = 0;
	Random rand;
	while (++i < 50) {
		cout << Buffer((rand.random<SS>()).buf, 16).toHexString() << endl;
	}
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

