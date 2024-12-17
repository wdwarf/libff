/**
 * @file ScopeGuard.h
 * @auth DuckyLi
 * @date 2016-11-28 15:26:14
 * @description
 */

#ifndef _FF_SCOPEGUARD_H_
#define _FF_SCOPEGUARD_H_

#include <ff/Noncopyable.h>
#include <ff/Utility.h>

NS_FF_BEG
template <typename F>
class __ScopeGuard;

template <typename F>
__ScopeGuard<typename std::decay<F>::type> ScopeGuard(F func);

template <typename F>
class __ScopeGuard : private Noncopyable {
 public:
  __ScopeGuard(__ScopeGuard<F>&& s)
      : m_func(std::move(s.m_func)),
        m_invoke(ff::Utility::Exchange(s.m_invoke, false)) {}

  ~__ScopeGuard() {
    if (m_invoke) m_func();
  }

 private:
  bool m_invoke = true;
  F m_func;
  __ScopeGuard(F&& func) : m_func(std::move(func)) {}
  friend __ScopeGuard<typename std::decay<F>::type> ScopeGuard<F>(F);
};

template <typename F>
__ScopeGuard<typename std::decay<F>::type> ScopeGuard(F func) {
  return __ScopeGuard<typename std::decay<F>::type>(std::forward<F>(func));
}

NS_FF_END

#endif /** _FF_SCOPEGUARD_H_ */
