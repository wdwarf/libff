/*
 * Timestamp.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#include <ff/Timestamp.h>
#include <cstring>
#include <sys/time.h>
#include <iostream>

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

