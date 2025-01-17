/*
 * String.h
 *
 *  Created on: Sep 5, 2016
 *      Author: ducky
 */

#ifndef FF_STRING_H_
#define FF_STRING_H_

#include <ff/List.h>
#include <ff/Object.h>
#include <ff/StringEncoding.h>

#include <list>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;

NS_FF_BEG

LIBFF_API std::string& ToLower(std::string& str);
LIBFF_API std::string ToLowerCopy(const std::string& str);
LIBFF_API std::string& ToUpper(std::string& str);
LIBFF_API std::string ToUpperCopy(const std::string& str);
LIBFF_API std::string& TrimLeft(std::string& str);
LIBFF_API std::string TrimLeftCopy(const std::string& str);
LIBFF_API std::string& TrimRight(std::string& str);
LIBFF_API std::string TrimRightCopy(const std::string& str);
LIBFF_API std::string& Trim(std::string& str);
LIBFF_API std::string TrimCopy(const std::string& str);
LIBFF_API std::string& Reverse(std::string& str);
LIBFF_API std::string ReverseCopy(const std::string& str);

LIBFF_API std::string& Replace(std::string& src, const std::string& find,
                               const std::string& replace,
                               bool ignoreCase = false);
LIBFF_API std::string ReplaceCopy(const std::string& src,
                                  const std::string& find,
                                  const std::string& replace,
                                  bool ignoreCase = false);
LIBFF_API std::string& ReplaceAll(std::string& src, const std::string& find,
                                  const std::string& replace,
                                  bool ignoreCase = false);
LIBFF_API std::string ReplaceAllCopy(const std::string& src,
                                     const std::string& find,
                                     const std::string& replace,
                                     bool ignoreCase = false);

LIBFF_API int Compare(const std::string& s1, const std::string& s2,
                      bool ignoreCase = false);

template <class T>
std::string Join(const T& strings, const std::string& delimiter) {
  std::stringstream s;
  for (auto& str : strings) {
    s << str << delimiter;
  }
  auto rt = s.str();
  return rt.substr(0, rt.length() - delimiter.length());
}

LIBFF_API int IndexOf(const std::string& src, const std::string& find,
                      bool ignoreCase = false);
LIBFF_API bool EndsWith(const std::string& src, const std::string& find,
                        bool ignoreCase = false);
LIBFF_API bool StartsWith(const std::string& src, const std::string& find,
                          bool ignoreCase = false);
LIBFF_API bool Match(const std::string& str, const std::string& reg);

enum StringCompressType { WithEmptyString, RemoveEmptyString };

class LIBFF_API IDelimiter {
 public:
  IDelimiter(const std::string& delimiterStr);
  virtual ~IDelimiter();

  virtual int Find(const std::string& str) const = 0;
  virtual int DelimiterSize() const = 0;

 protected:
  std::string m_delimiterStr;
};

class LIBFF_API IsAnyOf : public IDelimiter {
 public:
  IsAnyOf(const std::string& delimiter);

  int Find(const std::string& str) const;
  int DelimiterSize() const;
};

class LIBFF_API IsStringOf : public IDelimiter {
 public:
  IsStringOf(const std::string& delimiter);

  int Find(const std::string& str) const;
  int DelimiterSize() const;
};

LIBFF_API std::vector<std::string> Split(
    const std::string& text, const IDelimiter& delimiterChecker,
    StringCompressType compressType = WithEmptyString);

class LIBFF_API String : public std::string {
 public:
  using std::string::string;

  String() = default;
  ~String() = default;

  String(const std::wstring& wstr);
  String& operator=(const std::wstring& wstr);

  String(const std::string& str) : std::string(str) {}
  String(std::string&& str) : std::string(move(str)) {}

  operator std::wstring() const;

  String(const char* str, size_t len);

  bool equals(const String& str, bool isEgnoreCase = false) const;
  bool match(const String& regStr) const;

  String trimLeft() const;
  String trimRight() const;
  String trim() const;
  String toLower() const;
  String toUpper() const;
  String reverse() const;
  String replace(const String& find, const String& replace,
                 bool ignoreCase = false) const;
  String replaceAll(const String& find, const String& replace,
                    bool ignoreCase = false) const;
  int indexOf(const String& find, bool ignoreCase = false) const;
  bool endsWith(const String& find, bool ignoreCase = false) const;
  bool startsWith(const String& find, bool ignoreCase = false) const;

  std::vector<String> split(const IDelimiter& delimiterChecker,
                            StringCompressType compressType = WithEmptyString);

  int toInt(int base = 10, bool* ok = nullptr) const;
  long long toLongLong(int base = 10, bool* ok = nullptr) const;
  unsigned int toUInt(int base = 10, bool* ok = nullptr) const;
  unsigned long long toULongLong(int base = 10, bool* ok = nullptr) const;
  float toFloat(bool* ok = nullptr) const;
  double toDouble(bool* ok = nullptr) const;
  long double toLongDouble(bool* ok = nullptr) const;

  static String number(uint64_t value, int width = 0, int base = 10,
                       char fillChar = '0');
};

class LIBFF_API StringList : public List<String> {
 public:
  using List<String>::List;

  String join(const String& separator) const;
  size_type indexOf(const String& str, bool ignoreCase = false) const;
  bool contains(const String& str, bool ignoreCase = false) const;
};

NS_FF_END

#endif /* DUCKY_STRIING_STRING_H_ */
