/*
 * Synchronizable.h
 *
 *  Created on: Dec 16, 2019
 *      Author: root
 */

#ifndef FF_SYNCHRONIZABLE_H_
#define FF_SYNCHRONIZABLE_H_

#include <ff/Locker.h>
#include <ff/Object.h>

#include <condition_variable>
#include <functional>
#include <mutex>

NS_FF_BEG

class LIBFF_API Synchronizable {
 public:
  Synchronizable();
  virtual ~Synchronizable();

  using PredicateFunc = std::function<bool()>;

  void lock();
  bool trylock();
  void unlock();
  void wait();
  void wait(PredicateFunc predicate);
  bool wait(uint64_t msTimeout);
  void wait(uint64_t msTimeout, PredicateFunc predicate);
  void notifyOne();
  void notifyAll();

 private:
  std::mutex m_mutex;
  std::condition_variable m_cond;
};

typedef NS_FF::Locker<Synchronizable> Synchronize;

NS_FF_END

#endif /* FF_SYNCHRONIZABLE_H_ */
