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

#ifdef _WIN32
#else
#include <sys/time.h>
#endif

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

NS_FF_BEG

Timestamp::Timestamp() : m_ts(Now()) {}

Timestamp::Timestamp(uint64_t t) : m_ts(t) {}

Timestamp::Timestamp(const Timestamp &t) { this->m_ts = t.m_ts.load(); }

uint64_t Timestamp::Now() {
  return std::chrono::time_point_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now())
      .time_since_epoch()
      .count();
}

Timestamp &Timestamp::operator=(const Timestamp &t) {
  this->m_ts = t.m_ts.load();
  return *this;
}

bool Timestamp::operator==(const Timestamp &t) const {
  return (this->m_ts == t.m_ts);
}

bool Timestamp::operator!=(const Timestamp &t) const {
  return !this->operator==(t);
}

bool Timestamp::operator<(const Timestamp &t) const {
  return (this->m_ts < t.m_ts);
}

bool Timestamp::operator<=(const Timestamp &t) const {
  return (this->m_ts <= t.m_ts);
}

bool Timestamp::operator>(const Timestamp &t) const {
  return !this->operator<(t);
}

bool Timestamp::operator>=(const Timestamp &t) const {
  return (this->m_ts >= t.m_ts);
}

string Timestamp::toLocalString(const string &f) const {
  string format = f.empty() ? "%F %T" : f;
  stringstream str;
  str << DateTime(this->m_ts / 1000).toLocalString(format) << "." << setw(3)
      << setfill('0') << (m_ts % 1000);
  return str.str();
}

uint64_t Timestamp::get() const { return this->m_ts; }

Timestamp::operator uint64_t() const { return this->m_ts; }

NS_FF_END
