/*
 * StringWrapper.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/StringWrapper.h>

using namespace std;

namespace NS_FF {

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

std::ostream& operator<<(std::ostream& o, const StringWrapper& wp) {
	o << wp.toString();
	return o;
}

} /* namespace NS_FF */
