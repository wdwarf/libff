/*
 * StringWrapper.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/StringWrapper.h>

using namespace std;

NS_FF_BEG

StringWrapper::StringWrapper() {
}

StringWrapper::~StringWrapper() {
}

string StringWrapper::toString() const {
	return this->str();
}

StringWrapper::operator std::string() const {
	return this->str();
}

LIBFF_API std::ostream& operator<<(std::ostream& o, const StringWrapper& wp) {
	o << wp.toString();
	return o;
}

NS_FF_END
