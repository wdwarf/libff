/**
 * @file Numeric.h
 * @author DuckyLi
 * @date 2023-06-29 21:25:20
 * @description
 */

#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#include <ff/ff_config.h>

NS_FF_BEG

template <typename T>
bool IsEquals(T a, T b) {
  return std::fabs(a - b) <= std::numeric_limits<T>::epsilon();
}

NS_FF_END

#endif /** _NUMERIC_H_ */
