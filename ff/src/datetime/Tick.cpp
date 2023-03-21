/*
 * Tick.cpp
 *
 *  Created on: Nov 25, 2017
 *      Author: ducky
 */

#include <errno.h>
#include <ff/Tick.h>

#include <cstring>
#include <ctime>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else

#endif

using std::string;

NS_FF_BEG

#ifdef _WIN32

class Tick::TickImpl {
 public:
  TickImpl() {
    QueryPerformanceFrequency(&m_frq);
    QueryPerformanceCounter(&m_start);
  }

  tick_t tick() {
    QueryPerformanceCounter(&m_start);
    return m_start.QuadPart * double(1000) / m_frq.QuadPart;
  }

  tick_t tock() const {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (now.QuadPart - m_start.QuadPart) * double(1000) / m_frq.QuadPart;
  }

 private:
  LARGE_INTEGER m_frq;
  LARGE_INTEGER m_start;
};

#else

class Tick::TickImpl {
 public:
  TickImpl() : m_start(Tick::GetTickCount()) {}

  tick_t tick() {
    m_start = Tick::GetTickCount();
    return m_start;
  }

  tick_t tock() const { return Tick::GetTickCount() - m_start; }

 private:
  tick_t m_start;
};

#endif

Tick::Tick() : m_impl(new TickImpl) {}

Tick::~Tick() { delete m_impl; }

tick_t Tick::GetTickCount() {
#ifdef _WIN32
  return ::GetTickCount();
#else
  struct timespec ts;
  if (-1 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
    return -1;
  }
  return (tick_t(ts.tv_sec * 1000) + ts.tv_nsec / 1000000);
#endif
}

tick_t Tick::tick() { return this->m_impl->tick(); }

tick_t Tick::tock() const { return this->m_impl->tock(); }

NS_FF_END
