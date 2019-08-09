/*
 * Timestamp.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#include <ff/Timestamp.h>
#include <cstring>
#include <ctime>
#include <vector>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace NS_FF {

class Timestamp::TimestampImpl {
public:
	TimestampImpl() {
		memset(&ts, 0, sizeof(timespec));
	}

	void setCurrentTime() {
		clock_gettime(CLOCK_REALTIME, &ts);
	}

	bool operator==(const TimestampImpl& t) const {
		if (t.ts.tv_sec == this->ts.tv_sec) {
			return (t.ts.tv_nsec == this->ts.tv_nsec);
		}
		return false;
	}

	bool operator!=(const TimestampImpl& t) const {
		return !this->operator ==(t);
	}

	bool operator<(const TimestampImpl& t) const {
		if (this->ts.tv_sec < t.ts.tv_sec) {
			return true;
		} else if (this->ts.tv_sec == t.ts.tv_sec) {
			return (t.ts.tv_nsec < this->ts.tv_nsec);
		}
		return false;
	}

	bool operator<=(const TimestampImpl& t) const {
		if (this->ts.tv_sec < t.ts.tv_sec) {
			return true;
		} else
			return this->operator ==(t);
	}

	bool operator>(const TimestampImpl& t) const {
		return !this->operator <(t);
	}

	bool operator>=(const TimestampImpl& t) const {
		if (this->ts.tv_sec > t.ts.tv_sec) {
			return true;
		} else
			return this->operator ==(t);
	}

	string toLocalString() const {
		string format = "%F %T";
		vector<char> buf(format.length() * 4 + 10);
		tm tm_t;
		if (NULL == localtime_r(&this->ts.tv_sec, &tm_t)) {
			THROW_EXCEPTION(DateTimeException,
					string("localtime_r failed: ") + strerror(errno), errno);
		}
		strftime(&buf[0], buf.size(), format.c_str(), &tm_t);

		stringstream str;
		str << &buf[0] << "." << setw(3) << setfill('0')
				<< (this->ts.tv_nsec / 1000000);
		return str.str();
	}

private:
	timespec ts;
	friend class Timestamp;
};

Timestamp::Timestamp() :
		impl(new TimestampImpl) {
	//
}

Timestamp::Timestamp(const Timestamp& t) :
		impl(new TimestampImpl) {
	*this->impl = *t.impl;
}

Timestamp::~Timestamp() {
	delete this->impl;
}

Timestamp& Timestamp::operator=(const Timestamp& t) {
	*this->impl = *t.impl;
	return *this;
}

DateTime Timestamp::toDateTime() const {
	return DateTime(this->impl->ts.tv_sec);
}

string Timestamp::toLocalString() const {
	return this->impl->toLocalString();
}

Timestamp Timestamp::now() {
	Timestamp t;
	t.impl->setCurrentTime();
	return t;
}

bool Timestamp::operator==(const Timestamp& t) const {
	return this->impl->operator==(*t.impl);
}

bool Timestamp::operator!=(const Timestamp& t) const {
	return this->impl->operator!=(*t.impl);
}

bool Timestamp::operator<(const Timestamp& t) const {
	return this->impl->operator<(*t.impl);
}

bool Timestamp::operator<=(const Timestamp& t) const {
	return this->impl->operator<=(*t.impl);
}

bool Timestamp::operator>(const Timestamp& t) const {
	return this->impl->operator>(*t.impl);
}

bool Timestamp::operator>=(const Timestamp& t) const {
	return this->impl->operator>=(*t.impl);
}

} /* namespace NS_FF */

