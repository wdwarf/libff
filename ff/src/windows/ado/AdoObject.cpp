/*
 * AdoObject.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#include <ff/windows/AdoObject.h>

NS_FF_BEG

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

NS_FF_END
