/*
 * AdoObject.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#include <ff/windows/AdoObject.h>

NS_FF_BEG

namespace {

// class __Init__ {
//  public:
//   __Init__() { ::CoInitializeEx(NULL, COINIT_MULTITHREADED); }
// };
// static __Init__ __init__;

}  // namespace

AdoObject::AdoObject() {}

AdoObject::~AdoObject() {}

StringT AdoObject::BstrToStr(const bstr_t &str) {
  if (str.length() > 0) {
    return (const TCHAR *)str;
  }
  return _T("");
}

NS_FF_END
