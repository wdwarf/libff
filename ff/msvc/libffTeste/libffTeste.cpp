// libffTeste.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <ff/Object.h>
#include <ff/DateTime.h>
#include <ff/File.h>
#include <cstring>
#include <errno.h>

using namespace std;
using namespace ff;

namespace Test {
	class T : public Object {

	};
}

int main()
{
	Test::T t;
	cout << t.getClassName() << endl;
	cout << t.getFullClassName() << endl;
	DateTime dt = DateTime::now();
	cout << dt.toLocalString() << endl;

	File f("D:\\tools");
	cout << "is exists: " << f.isExists() << endl;
	cout << "is exe: " << f.isExecutable() << endl;
	cout << "is dir: " << f.isDirectory() << endl;
	cout << "size: " << f.getSize() << endl;
	cout << "copy: " << f.copyTo("d:\\tools2", true) << endl;
	cout << "end" << endl;
	cin.get();
	cout << "rm: " << File("d:\\tools2").remove() << endl;
	
	cin.get();
    return 0;
}

