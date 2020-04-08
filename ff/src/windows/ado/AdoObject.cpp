/*
 * AdoObject.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#include <ff/windows/AdoObject.h>

namespace NS_FF {

AdoObject::AdoObject() {
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

AdoObject::~AdoObject() {

}

StringT AdoObject::BstrToStr(const bstr_t &str) {
	if (str.length() > 0) {
		return (const TCHAR*) str;
	}
	return _T("");
}

} /* namespace NS_FF */
