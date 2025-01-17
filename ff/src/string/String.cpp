/*
 * String.cpp
 *
 *  Created on: Sep 5, 2016
 *      Author: ducky
 */

#include <ff/Exception.h>
#include <ff/String.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp _strcmpi
#endif

NS_FF_BEG

inline int _isspace_(int c) {
  int re = isspace(c);
  if (0 == re) {
    re = ('\0' == c);
  }
  return re;
}

inline int _not_space_(int c) { return (0 == _isspace_(c)); }

string& ToLower(string& str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

string ToLowerCopy(const string& str) {
  string re = str;
  ToLower(re);
  return re;
}

string& ToUpper(string& str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

string ToUpperCopy(const string& str) {
  string re = str;
  ToUpper(re);
  return re;
}

string& TrimLeft(string& str) {
  string::iterator p = find_if(str.begin(), str.end(), _not_space_);
  str.erase(str.begin(), p);
  return str;
}

string TrimLeftCopy(const string& str) {
  string newStr = str;
  return TrimLeft(newStr);
}

string& TrimRight(string& str) {
  string::reverse_iterator p = find_if(str.rbegin(), str.rend(), _not_space_);
  str.erase(p.base(), str.end());
  return str;
}

string TrimRightCopy(const string& str) {
  string newStr = str;
  return TrimRight(newStr);
}

string& Trim(string& str) { return TrimRight(TrimLeft(str)); }

string TrimCopy(const string& str) {
  string newStr = str;
  return Trim(newStr);
}

string& Reverse(std::string& str) {
  reverse(str.begin(), str.end());
  return str;
}

string ReverseCopy(const std::string& str) {
  string newStr = str;
  return Reverse(newStr);
}

string& Replace(string& src, const string& find, const string& replace,
                bool ignoreCase) {
  string::size_type pos = string::npos;
  if (ignoreCase) {
    string s = src;
    string f = find;
    ToLower(s);
    ToLower(f);
    pos = s.find(f);
  } else {
    pos = src.find(find);
  }

  if (pos != string::npos) {
    src.replace(pos, find.length(), replace);
  }

  return src;
}

string ReplaceCopy(const string& src, const string& find, const string& replace,
                   bool ignoreCase) {
  string str = src;
  return Replace(str, find, replace, ignoreCase);
}

string& ReplaceAll(string& src, const string& find, const string& replace,
                   bool ignoreCase) {
  string::size_type pos = 0;
  string s = src;
  string f = find;
  if (ignoreCase) {
    ToLower(s);
    ToLower(f);
  }

  do {
    pos = s.find(f, pos);

    if (pos != string::npos) {
      src.replace(pos, find.length(), replace);
      s.replace(pos, find.length(), replace);
    } else {
      break;
    }
    pos += replace.length();
  } while (true);
  return src;
}

string ReplaceAllCopy(const string& src, const string& find,
                      const string& replace, bool ignoreCase) {
  string str = src;
  return ReplaceAll(str, find, replace, ignoreCase);
}

int Compare(const std::string& s1, const std::string& s2, bool ignoreCase) {
  if (!ignoreCase) {
    return s1 > s2 ? 1 : s1 < s2 ? -1 : 0;
  }

  auto s1l = ToLowerCopy(s1);
  auto s2l = ToLowerCopy(s2);
  return s1l > s2l ? 1 : s1l < s2l ? -1 : 0;
}

int IndexOf(const string& src, const string& find, bool ignoreCase) {
  int re = -1;
  string::size_type pos = string::npos;
  if (ignoreCase) {
    string s = ToLowerCopy(src);
    string f = ToLowerCopy(find);
    pos = s.find(f);
  } else {
    pos = src.find(find);
  }

  if (pos != string::npos) {
    re = pos;
  }

  return re;
}

bool EndsWith(const std::string& src, const std::string& find,
              bool ignoreCase) {
  if (src.length() < find.length()) return false;
  string s = src.substr(src.length() - find.length());
  if (ignoreCase) {
    return ToLowerCopy(s) == ToLowerCopy(find);
  }

  return s == find;
}

bool StartsWith(const std::string& src, const std::string& find,
                bool ignoreCase) {
  if (src.length() < find.length()) return false;
  string s = src.substr(0, find.length());
  if (ignoreCase) {
    return ToLowerCopy(s) == ToLowerCopy(find);
  }

  return s == find;
}

bool Match(const std::string& str, const std::string& reg) {
  return regex_match(str, regex(reg));
}

/////////////////////////////////////////////////////////////////
/// Split
/////////////////////////////////////////////////////////////////

IDelimiter::IDelimiter(const std::string& delimiterStr)
    : m_delimiterStr(delimiterStr) {}

IDelimiter::~IDelimiter() {}

IsAnyOf::IsAnyOf(const std::string& delimiter) : IDelimiter(delimiter) {}

int IsAnyOf::Find(const std::string& str) const {
  for (string::size_type i = 0; i < str.length(); ++i) {
    if (string::npos != this->m_delimiterStr.find(str[i])) {
      return i;
    }
  }
  return -1;
}

int IsAnyOf::DelimiterSize() const { return 1; }

IsStringOf::IsStringOf(const std::string& delimiter) : IDelimiter(delimiter) {}

int IsStringOf::Find(const std::string& str) const {
  string::size_type pos = str.find(this->m_delimiterStr);
  if (string::npos != pos) {
    return pos;
  }

  return -1;
}

int IsStringOf::DelimiterSize() const { return this->m_delimiterStr.length(); }

std::vector<std::string> Split(const std::string& text,
                               const IDelimiter& delimiterChecker,
                               StringCompressType compressType) {
  std::vector<std::string> texts;
  std::string srcText = text;

  while (!srcText.empty()) {
    int pos = delimiterChecker.Find(srcText);
    if (pos >= 0) {
      auto findStr = srcText.substr(0, pos);
      srcText = srcText.substr(pos + delimiterChecker.DelimiterSize());

      if (WithEmptyString == compressType) {
        texts.push_back(findStr);
        continue;
      }

      Trim(findStr);
      if (!findStr.empty()) {
        texts.push_back(findStr);
      }
      continue;
    }

    texts.push_back(srcText);
    break;
  }
  return texts;
}

/////////////////////////////////////////////////////////////////
/// class String
/////////////////////////////////////////////////////////////////

String::String(const char* str, size_t len) : string(str, len) {}

bool String::equals(const String& str, bool isEgnoreCase) const {
  return (isEgnoreCase ? 0 == strcasecmp(this->c_str(), str.c_str())
                       : 0 == strcmp(this->c_str(), str.c_str()));
}

String::String(const std::wstring& wstr) { *this = ToMbs(wstr); }

String& String::operator=(const std::wstring& wstr) {
  *this = ToMbs(wstr);
  return *this;
}

String::operator std::wstring() const { return ToWs(*this); }

bool String::match(const String& regStr) const { return Match(*this, regStr); }

String String::trimLeft() const { return TrimLeftCopy(*this); }

String String::trimRight() const { return TrimRightCopy(*this); }

String String::trim() const { return TrimCopy(*this); }

String String::toLower() const { return ToLowerCopy(*this); }

String String::toUpper() const { return ToUpperCopy(*this); }

String String::reverse() const { return ReverseCopy(*this); }

String String::replace(const String& find, const String& replace,
                       bool ignoreCase) const {
  return ReplaceCopy(*this, find, replace, ignoreCase);
}

String String::replaceAll(const String& find, const String& replace,
                          bool ignoreCase) const {
  return ReplaceAllCopy(*this, find, replace, ignoreCase);
}

int String::indexOf(const String& find, bool ignoreCase) const {
  return IndexOf(*this, find, ignoreCase);
}

bool String::endsWith(const String& find, bool ignoreCase) const {
  return EndsWith(*this, find, ignoreCase);
}

bool String::startsWith(const String& find, bool ignoreCase) const {
  return StartsWith(*this, find, ignoreCase);
}

std::vector<String> String::split(const IDelimiter& delimiterChecker,
                                  StringCompressType compressType) {
  std::vector<String> texts;
  std::string srcText = *this;

  while (!srcText.empty()) {
    int pos = delimiterChecker.Find(srcText);
    if (pos >= 0) {
      auto findStr = srcText.substr(0, pos);
      srcText = srcText.substr(pos + delimiterChecker.DelimiterSize());

      if (WithEmptyString == compressType) {
        texts.push_back(findStr);
        continue;
      }

      Trim(findStr);
      if (!findStr.empty()) {
        texts.push_back(findStr);
      }
      continue;
    }

    texts.push_back(srcText);
    break;
  }
  return texts;
}

int String::toInt(int base, bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  int r = strtol(str, &endP, base);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

long long String::toLongLong(int base, bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  long long r = strtoll(str, &endP, base);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

unsigned int String::toUInt(int base, bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  unsigned int r = strtoul(str, &endP, base);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

unsigned long long String::toULongLong(int base, bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  unsigned long long r = strtoull(str, &endP, base);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

float String::toFloat(bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  float r = strtof(str, &endP);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

double String::toDouble(bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  double r = strtod(str, &endP);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

long double String::toLongDouble(bool* ok) const {
  const char* str = this->c_str();
  char* endP = nullptr;
  long double r = strtold(str, &endP);
  if (nullptr != ok) {
    *ok = (endP == (str + this->length()));
  }
  return r;
}

String String::number(uint64_t value, int width, int base, char fillChar) {
  stringstream str;

  switch (base) {
    case 8:
      str >> oct;
      break;
    case 16:
      str >> hex;
      break;
    default:
      str >> dec;
      break;
  }
  if (width > 0) str.width(width);
  str.fill(fillChar);

  str << value;
  string s;
  str >> s;
  return s;
}

String StringList::join(const String& separator) const {
  std::stringstream str;
  auto it = this->begin();
  while (it != this->end()) {
    str << *it;
    ++it;
    if (it != this->end()) str << separator;
  }
  return str.str();
}

StringList::size_type StringList::indexOf(const String& str,
                                          bool ignoreCase) const {
  if (!ignoreCase) return List<String>::indexOf(str);

  auto lowerStr = str.toLower();
  for (size_type i = 0; i < size(); ++i) {
    if (lowerStr == at(i).toLower()) return i;
  }

  return static_cast<size_type>(-1);
}

bool StringList::contains(const String& str, bool ignoreCase) const {
  return static_cast<size_type>(-1) != indexOf(str, ignoreCase);
}

NS_FF_END
