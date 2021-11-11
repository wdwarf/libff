/*
 * StringWrapper.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_STRINGWRAPPER_H_
#define FF_STRINGWRAPPER_H_

#include <ff/ff_config.h>

#include <initializer_list>
#include <sstream>
#include <string>

NS_FF_BEG

class LIBFF_API StringWrapper {
 public:
  StringWrapper();
  virtual ~StringWrapper();

  template <class T>
  StringWrapper(const T& t) {
    m_stream << t;
  }

  template <class T>
  StringWrapper& operator()(const T& t) {
    m_stream << t;
    return *this;
  }

  template <class T>
  StringWrapper& operator<<(const T& t) {
    return this->operator()(t);
  }

  std::string toString() const;
  operator std::string() const;

  friend LIBFF_API std::ostream& operator<<(std::ostream& o,
                                            const StringWrapper& wp);

 private:
  std::stringstream m_stream;
};

typedef StringWrapper SW;

NS_FF_END

#endif /* FF_STRINGWRAPPER_H_ */
