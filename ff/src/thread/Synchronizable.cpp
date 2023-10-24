/*
 * Synchronizable.cpp
 *
 *  Created on: Dec 16, 2019
 *      Author: root
 */

#include <ff/Synchronizable.h>

using namespace std;

NS_FF_BEG

Synchronizable::Synchronizable() {}

Synchronizable::~Synchronizable() {}

void Synchronizable::lock() { this->m_mutex.lock(); }

void Synchronizable::unlock() { this->m_mutex.unlock(); }

bool Synchronizable::trylock() { return this->m_mutex.try_lock(); }

void Synchronizable::wait() {
  std::unique_lock<std::mutex> lk(m_mutex);
  m_cond.wait(lk);
}

void Synchronizable::wait(PredicateFunc predicate) {
  std::unique_lock<std::mutex> lk(m_mutex);
  m_cond.wait(lk, predicate);
}

bool Synchronizable::wait(uint64_t msTimeout) {
  std::unique_lock<std::mutex> lk(m_mutex);
  return (cv_status::no_timeout ==
          m_cond.wait_for(lk, chrono::milliseconds(msTimeout)));
}

void Synchronizable::wait(uint64_t msTimeout, PredicateFunc predicate) {
  std::unique_lock<std::mutex> lk(m_mutex);
  m_cond.wait_for(lk, chrono::milliseconds(msTimeout), predicate);
}

void Synchronizable::notifyOne() {
  std::unique_lock<std::mutex> lk(m_mutex);
  m_cond.notify_one();
}

void Synchronizable::notifyAll() {
  std::unique_lock<std::mutex> lk(m_mutex);
  m_cond.notify_all();
}

NS_FF_END
