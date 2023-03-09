/**
 * @file Delayable.h
 * @auth DuckyLi
 * @date 2023-03-09 21:24:13
 * @description
 */

#ifndef _DELAYABLE_H_
#define _DELAYABLE_H_

#include <ff/ff_config.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>

NS_FF_BEG

class LIBFF_API Delayable {
 public:
  Delayable();
  ~Delayable();

  bool delay(int64_t delayMs);
  void cancel();

 private:
  std::atomic_bool m_canceled;
  std::mutex m_mutex;
  std::condition_variable m_cond;
};

NS_FF_END

#endif /** _DELAYABLE_H_ */
