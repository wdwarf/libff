/**
 * @file Utility.h
 * @author liyawu
 * @date 2024-12-17 10:39:26
 * @description
 */

#ifndef _FF_UTILITY_H
#define _FF_UTILITY_H

#include <ff/ff_config.h>

NS_FF_BEG

namespace Utility {

/**
 * 将newValue赋给t，并返回t的旧值。
 */
template <typename T, typename U = T>
T Exchange(T& t, U&& newValue) {
  T old = std::move(t);
  t = std::forward<U>(newValue);
  return old;
}

};  // namespace Utility

NS_FF_END

#endif  // _FF_UTILITY_H
