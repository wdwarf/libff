/**
 * @file Delayable.cpp
 * @auth DuckyLi
 * @date 2023-03-09 21:24:43
 * @description
 */
#include <ff/Delayable.h>
#include <ff/Tick.h>

#include <thread>

NS_FF_BEG

Delayable::Delayable() : m_canceled(true) {}

Delayable::~Delayable() { this->cancel(); }

bool Delayable::delay(int64_t delayMs) {
  m_canceled = false;
  auto waitMs = delayMs;
  Tick tick;
  tick.tick();
  while (waitMs > 0) {
    std::unique_lock<std::mutex> lk(m_mutex);
    if (std::cv_status::timeout ==
        m_cond.wait_for(lk, std::chrono::milliseconds(waitMs)))
      break;
    if (m_canceled) return false;
    waitMs = delayMs - tick.tock();
  }
  return true;
}

void Delayable::cancel() {
  m_canceled = true;
  std::lock_guard<std::mutex> lk(m_mutex);
  m_cond.notify_one();
}

NS_FF_END
