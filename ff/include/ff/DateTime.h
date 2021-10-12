/*
 * DateTime.h
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#ifndef FF_DATETIME_DATETIME_H_
#define FF_DATETIME_DATETIME_H_

#include <ff/Object.h>
#include <ff/Exception.h>
#include <string>
#include <ctime>

#define MILLI_SECS_PER_SEC	1000
#define MICRO_SECS_PER_SEC	1000000
#define NANO_SECS_PER_SEC	1000000000

NS_FF_BEG

EXCEPTION_DEF(DateTimeException);

class LIBFF_API DateTime {
public:
	DateTime();
	DateTime(time_t t);
	DateTime(const DateTime& t);
	virtual ~DateTime();

	int getLocalYear() const;
	int getLocalMonth() const;
	int getLocalMDay() const;
	int getLocalHour() const;
	int getLocalMinute() const;
	int getLocalSecond() const;
	int getLocalWeek() const;
	int getLocalYearDay() const;
	std::string getLocalDate() const;
	std::string getLocalTime() const;
	std::string toLocalString(const std::string& format = "") const;
	tm toLocalTm() const;

	int getYear() const;
	int getMonth() const;
	int getMDay() const;
	int getHour() const;
	int getMinute() const;
	int getSecond() const;
	int getWeek() const;
	int getYearDay() const;
	std::string getDate() const;
	std::string getTime() const;
	std::string toString(const std::string& format = "") const;
	tm toTm() const;

	DateTime& operator=(const time_t& t);
	operator time_t() const;
	bool operator==(const DateTime& t) const;
	bool operator!=(const DateTime& t) const;
	bool operator<(const DateTime& t) const;
	bool operator<=(const DateTime& t) const;
	bool operator>(const DateTime& t) const;
	bool operator>=(const DateTime& t) const;
	time_t operator-(const DateTime& t) const;

	bool setSystemTime();

	static DateTime Now();

private:
	time_t t;
};


NS_FF_END

#endif /* FF_DATETIME_DATETIME_H_ */
