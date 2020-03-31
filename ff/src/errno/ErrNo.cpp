/*
 * ErrNo.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/ErrNo.h>
#include <cstring>
#include <ff/StringWrapper.h>

using namespace std;

namespace NS_FF {

namespace {

ErrorNo_t _GetErrorNo() {
#ifdef _WIN32
	return ::GetLastError();
#else
	return errno;
#endif
}

}

ErrNo ErrNo::LastError() {
	return ErrNo();
}

ErrNo::ErrNo(ErrorNo_t errNo) {
	this->m_errNo = (0 == errNo) ? _GetErrorNo() : errNo;
}

ErrorNo_t ErrNo::getErrorNo() const {
	return this->m_errNo;
}

ErrNo::operator ErrorNo_t() const {
	return this->getErrorNo();
}

std::string ErrNo::getErrorStr() const {
#ifdef _WIN32
	LPTSTR lpBuffer = NULL;
	if (0 == FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		this->m_errNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpBuffer,
		0,
		NULL
	)){
		// return SW("Undefined errorNo(", this->m_errNo, ")");
		return SW("Undefined errorNo(");
	}

	std::string err = lpBuffer;
	LocalFree(lpBuffer);
	return err;
#else
	return strerror(this->m_errNo);
#endif
}

ErrNo::operator std::string() const {
	return this->getErrorStr();
}

} /* namespace NS_FF */
