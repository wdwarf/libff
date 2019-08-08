/*
 * DateTime.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#include <ff/DateTime.h>
#include <sys/time.h>
#include <errno.h>
#include <vector>
#include <ctime>
#include <cstring>
#include <sstream>

using namespace std;

namespace NS_FF {

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
	if (NULL == localtime_r(&t, &tm_t)) {
		THROW_EXCEPTION(DateTimeException,
				string("localtime_r failed: ") + strerror(errno), errno);
	}
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
	if (NULL == gmtime_r(&t, &tm_t)) {
		THROW_EXCEPTION(DateTimeException,
				string("gmtime_r failed: ") + strerror(errno), errno);
	}
	strftime(&buf[0], buf.size(), f.c_str(), &tm_t);
	return &buf[0];
}

tm DateTime::toLocalTm() const {
	tm tm_t;
	if (NULL == localtime_r(&t, &tm_t)) {
		THROW_EXCEPTION(DateTimeException,
				string("localtime_r failed: ") + strerror(errno), errno);
	}
	return tm_t;
}

tm DateTime::toTm() const {
	tm tm_t;
	if (NULL == gmtime_r(&t, &tm_t)) {
		THROW_EXCEPTION(DateTimeException,
				string("gmtime_r failed: ") + strerror(errno), errno);
	}
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

DateTime DateTime::now() {
	return DateTime(time(0));
}

void DateTime::setSystemTime() {
	timeval val;
	val.tv_sec = this->t;
	val.tv_usec = 0;
	if (0 != settimeofday(&val, nullptr)) {
		THROW_EXCEPTION(DateTimeException,
				string("set system time failed: ") + strerror(errno), errno);
	}
}

} /* namespace NS_FF */

