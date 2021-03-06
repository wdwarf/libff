/*
 * Timestamp.h
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#ifndef FF_TIMESTAMP_H_
#define FF_TIMESTAMP_H_

#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/DateTime.h>

NS_FF_BEG

EXCEPTION_DEF(TimestampException);

class LIBFF_API Timestamp {
public:
	Timestamp();
	Timestamp(const Timestamp& t);
	virtual ~Timestamp();

	Timestamp& operator=(const Timestamp& t);

	DateTime toDateTime() const;
	std::string toLocalString(const std::string& f = "") const;

	static Timestamp now();

	bool operator==(const Timestamp& t) const;
	bool operator!=(const Timestamp& t) const;
	bool operator<(const Timestamp& t) const;
	bool operator<=(const Timestamp& t) const;
	bool operator>(const Timestamp& t) const;
	bool operator>=(const Timestamp& t) const;
private:
	class TimestampImpl;
	TimestampImpl* impl;
};

NS_FF_END

#endif /* FF_TIMESTAMP_H_ */
