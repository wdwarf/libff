/*
 * DateTime.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#include <ff/DateTime.h>

#ifdef _WIN32
#else
#include <sys/time.h>
#endif

#include <errno.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstring>
#include <sstream>

using namespace std;

#ifdef _WIN32
#define __localtime__(t, tm)	localtime_s(tm, t)
#define __gmtime__(t, tm)	gmtime_s(tm, t)

#else
#define __localtime__(t, tm)	localtime_r(t, tm)
#define __gmtime__(t, tm)	gmtime_r(t, tm)
#endif

NS_FF_BEG

DateTime::DateTime() :
		t(0) {
}

DateTime::DateTime(const DateTime& t) {
	this->t = t.t;
}

DateTime::DateTime(time_t t_) :
		t(t_) {
}

DateTime::~DateTime() {

}

string DateTime::getLocalDate() const {
	return this->toLocalString("%F");
}

string DateTime::getLocalTime() const {
	return this->toLocalString("%T");
}

string DateTime::getDate() const {
	return this->toString("%F");
}

string DateTime::getTime() const {
	return this->toString("%T");
}

string DateTime::toLocalString(const string& format) const {
	string f = format;
	if (f.empty()) {
		f = "%F %T";
	}
	vector<char> buf(f.length() * 4 + 10);
	tm tm_t;
	__localtime__(&t, &tm_t);
	strftime(&buf[0], buf.size(), f.c_str(), &tm_t);
	return &buf[0];
}

string DateTime::toString(const string& format) const {
	string f = format;
	if (f.empty()) {
		f = "%F %T";
	}
	vector<char> buf(f.length() * 4 + 10);
	tm tm_t;
	__gmtime__(&t, &tm_t);
	strftime(&buf[0], buf.size(), f.c_str(), &tm_t);
	return &buf[0];
}

tm DateTime::toLocalTm() const {
	tm tm_t;
	__localtime__(&t, &tm_t);
	return tm_t;
}

tm DateTime::toTm() const {
	tm tm_t;
	__gmtime__(&t, &tm_t);
	return tm_t;
}

int DateTime::getLocalYear() const {
	return this->toLocalTm().tm_year + 1900;
}

int DateTime::getLocalMonth() const {
	return this->toLocalTm().tm_mon + 1;
}

int DateTime::getLocalMDay() const {
	return this->toLocalTm().tm_mday;
}

int DateTime::getLocalHour() const {
	return this->toLocalTm().tm_hour;
}

int DateTime::getLocalMinute() const {
	return this->toLocalTm().tm_min;
}

int DateTime::getLocalSecond() const {
	return this->toLocalTm().tm_sec;
}

int DateTime::getLocalWeek() const {
	return this->toLocalTm().tm_wday;
}

int DateTime::getLocalYearDay() const {
	return this->toLocalTm().tm_yday;
}

int DateTime::getYear() const {
	return this->toTm().tm_year + 1900;
}

int DateTime::getMonth() const {
	return this->toTm().tm_mon + 1;
}

int DateTime::getMDay() const {
	return this->toTm().tm_mday;
}

int DateTime::getHour() const {
	return this->toTm().tm_hour;
}

int DateTime::getMinute() const {
	return this->toTm().tm_min;
}

int DateTime::getSecond() const {
	return this->toTm().tm_sec;
}

int DateTime::getWeek() const {
	return this->toTm().tm_wday;
}

int DateTime::getYearDay() const {
	return this->toTm().tm_yday;
}

DateTime& DateTime::operator=(const time_t& t) {
	this->t = t;
	return *this;
}

DateTime::operator time_t() const {
	return this->t;
}

bool DateTime::operator==(const DateTime& t) const {
	return (t.t == this->t);
}

bool DateTime::operator!=(const DateTime& t) const {
	return (this->t != t.t);
}

bool DateTime::operator<(const DateTime& t) const {
	return (t.t < this->t);
}

bool DateTime::operator<=(const DateTime& t) const {
	return (t.t <= this->t);
}

bool DateTime::operator>(const DateTime& t) const {
	return (this->t > t.t);
}

bool DateTime::operator>=(const DateTime& t) const {
	return (t.t >= this->t);
}

long DateTime::operator-(const DateTime& t) const {
	return (this->t - t.t);
}

DateTime DateTime::Now() {
	return DateTime(time(0));
}

bool DateTime::setSystemTime() {
#ifdef _WIN32
	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(sysTime));
	auto t = this->toTm();
	sysTime.wYear = t.tm_year + 1900;
	sysTime.wMonth = t.tm_mon + 1;
	sysTime.wDay = t.tm_mday;
	sysTime.wHour = t.tm_hour;
	sysTime.wMinute = t.tm_min;
	sysTime.wSecond = t.tm_sec;
	sysTime.wDayOfWeek = t.tm_wday;
	return (TRUE == ::SetSystemTime(&sysTime));
#else
	timeval val;
	val.tv_sec = this->t;
	val.tv_usec = 0;
	return (0 == settimeofday(&val, nullptr));
#endif
}

NS_FF_END

