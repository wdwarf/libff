/*
 * Object.cpp
 *
 *  Created on: Oct 14, 2016
 *      Author: ducky
 */

#include <ff/Object.h>
#include <typeinfo>
#include <sstream>
#include <cmath>
#include <cstring>
#include <vector>

using namespace std;

namespace {

string ParseClassNameWithNS(const string& fullName) {
	stringstream str;
	str << fullName;

	if (fullName.empty() || 'N' != str.get())
		return fullName;

	string className;
	while (true) {
		int len = 0;
		str >> len;
		if (len <= 0)
			break;

		vector<char> buf(len + 1);
		memset(&buf[0], 0, buf.size());
		str.read(&buf[0], len);

		className = className.empty() ? &buf[0] : (className + "::" + &buf[0]);
	}

	return className;
}

int NumLen(long n){
	return (int)log10(n) + 1;
}

}

namespace NS_FF {

Object::Object() {

}

Object::~Object() {

}

string Object::getClassName() const {
	string fullName = this->getFullClassName();
	string::size_type pos = fullName.find_last_of(":");
	return (string::npos != pos) ? fullName.substr(pos + 1) : fullName;
}

string Object::getFullClassName() const {
	string fullName = typeid(*this).name();
#ifdef _WIN32
	const string hdr = "class ";
	if (fullName.find(hdr) != string::npos)
	{
		fullName = fullName.substr(hdr.length());
	}
	return fullName;
#else
	if('N' == fullName[0])
		return ParseClassNameWithNS(fullName);

	stringstream str;
	str << fullName;
	int len = 0;
	str >> len;

	return len > 0 ? fullName.substr(NumLen(len), len) : fullName;
#endif
}

unsigned long Object::CppVeraion() {
	return __cplusplus;
}

} /* namespace NS_FF */
