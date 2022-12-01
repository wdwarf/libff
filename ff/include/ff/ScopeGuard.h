/**
 * @file ScopeGuard.h
 * @auth DuckyLi
 * @date 2016-11-28 15:26:14
 * @description
 */

#ifndef _FF_SCOPEGUARD_H_
#define _FF_SCOPEGUARD_H_

#include <ff/Noncopyable.h>
#include <ff/ff_config.h>

NS_FF_BEG

template <class Func>
class __ScopeGuard : private Noncopyable {
 public:
  __ScopeGuard(Func&& func) : m_func(std::move(func)) {}
  __ScopeGuard(const Func& func) : m_func(func) {}
  __ScopeGuard(__ScopeGuard<Func>&& s) : m_func(std::move(s.m_func)) {}
  ~__ScopeGuard() { m_func(); }

 private:
  Func m_func;
};

template <class Func>
__ScopeGuard<typename std::decay<Func>::type> ScopeGuard(Func&& func) {
  return __ScopeGuard<typename std::decay<Func>::type>(
      std::forward<Func>(func));
}

NS_FF_END

#endif /** _FF_SCOPEGUARD_H_ */
