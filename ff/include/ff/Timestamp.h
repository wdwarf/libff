/*
 * Timestamp.h
 *
 *  Created on: Nov 23, 2017
 *      Author: ducky
 */

#ifndef FF_TIMESTAMP_H_
#define FF_TIMESTAMP_H_

#include <ff/DateTime.h>
#include <ff/Exception.h>
#include <ff/Object.h>

#include <atomic>

NS_FF_BEG

EXCEPTION_DEF(TimestampException);

class LIBFF_API Timestamp {
 public:
  Timestamp();
  Timestamp(uint64_t t);
  Timestamp(const Timestamp& t);

  Timestamp& operator=(const Timestamp& t);

  uint64_t get() const;
  operator uint64_t() const;
  DateTime toDateTime() const;
  std::string toLocalString(const std::string& f = "") const;

  bool operator==(const Timestamp& t) const;
  bool operator!=(const Timestamp& t) const;
  bool operator<(const Timestamp& t) const;
  bool operator<=(const Timestamp& t) const;
  bool operator>(const Timestamp& t) const;
  bool operator>=(const Timestamp& t) const;

  static uint64_t Now();

 private:
  std::atomic<uint64_t> m_ts;
};

NS_FF_END

#endif /* FF_TIMESTAMP_H_ */
