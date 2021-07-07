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

#ifdef _WIN32
#include <Windows.h>
#else
#include <iconv.h>
#endif

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
  string::iterator p =
      find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(_isspace_)));
  str.erase(str.begin(), p);
  return str;
}

string TrimLeftCopy(const string& str) {
  string newStr = str;
  return TrimLeft(newStr);
}

string& TrimRight(string& str) {
  string::reverse_iterator p =
      find_if(str.rbegin(), str.rend(), not1(ptr_fun<int, int>(_isspace_)));
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

int IndexOf(const string& src, const string& find, bool ignoreCase) {
  int re = -1;
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

unsigned int HexAToI(char x) {
  if ((x >= 48) && (x <= 57))
    x -= 48; /* 0-9 */
  else if ((x >= 97) && (x <= 102))
    x -= 87; /* a-f */
  else if ((x >= 65) && (x <= 70))
    x -= 55; /* A-F */
  else {
    stringstream str;
    str << "Invalid hex character[" << x << "]";
    THROW_EXCEPTION(Exception, str.str(), x);
  }
  return x;
}

char IToHexA(unsigned int x) {
  if (x >= 0 && x <= 9) {
    x += 48;
  }
  if (x >= 10 && x <= 15) {
    x += 55;
  } else {
    stringstream str;
    str << "Invalid hex number[" << x << "]";
    THROW_EXCEPTION(Exception, str.str(), x);
  }

  return x;
}

#ifdef WIN32

std::string Utf8ToGbk(const std::string& str) {
  int len = MultiByteToWideChar(CP_UTF8, 0, srcStr.c_str(), -1, NULL, 0);
  wchar_t* wszGBK = new wchar_t[len + 1];
  memset(wszGBK, 0, len * 2 + 2);
  MultiByteToWideChar(CP_UTF8, 0, srcStr.c_str(), -1, wszGBK, len);
  len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
  char* szGBK = new char[len + 1];
  memset(szGBK, 0, len + 1);
  WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
  string strTemp(szGBK);
  if (wszGBK) delete[] wszGBK;
  if (szGBK) delete[] szGBK;
  return strTemp;
}

std::string GbkToUtf8(const std::string& strSrc) {
  int len = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), -1, NULL, 0);
  wchar_t* wstr = new wchar_t[len + 1];
  memset(wstr, 0, len + 1);
  MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), -1, wstr, len);
  len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  char* str = new char[len + 1];
  memset(str, 0, len + 1);
  WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
  string strTemp = str;
  if (wstr) delete[] wstr;
  if (str) delete[] str;
  return strTemp;
}

#else

string GbkToUtf8(const std::string& srcStr) {
  iconv_t cd = iconv_open("utf8", "gbk");
  if (nullptr == cd) return "";

  auto pSrc = srcStr.c_str();
  size_t srcLen = srcStr.length();
  char** pin = (char**)&pSrc;
  size_t bufSize = srcLen * 1.5;
  vector<char> outBuf(bufSize);
  memset(&outBuf[0], 0, bufSize);
  char* pout = &outBuf[0];

  iconv(cd, pin, &srcLen, &pout, &bufSize);
  iconv_close(cd);

  return string(&outBuf[0]);
}

string Utf8ToGbk(const std::string& srcStr) {
  iconv_t cd = iconv_open("gbk", "utf8");
  if (nullptr == cd) return "";

  auto pSrc = srcStr.c_str();
  size_t srcLen = srcStr.length();
  char** pin = (char**)&pSrc;
  size_t bufSize = srcLen * 1.5;
  vector<char> outBuf(bufSize);
  memset(&outBuf[0], 0, bufSize);
  char* pout = &outBuf[0];

  iconv(cd, pin, &srcLen, &pout, &bufSize);
  iconv_close(cd);

  return string(&outBuf[0]);
}

#endif

std::wstring ToWs(const std::string& str) {
  char* oldLocale = setlocale(LC_ALL, "");
  auto len = str.length();
  vector<wchar_t> buf(len + 1);
  std::fill(buf.begin(), buf.end(), 0);
  mbstowcs(&buf[0], str.c_str(), len);
  buf[len] = '\0';
  setlocale(LC_ALL, oldLocale);
  return wstring(&buf[0]);
}

std::string ToMbs(const std::wstring& str) {
  auto len = str.length() * 3;
  vector<char> buf(len + 1);
  std::fill(buf.begin(), buf.end(), 0);
  char* oldLocale = setlocale(LC_ALL, "");
  wcstombs(&buf[0], str.c_str(), len);
  buf[len] = '\0';
  setlocale(LC_ALL, oldLocale);
  return string(&buf[0]);
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

NS_FF_END
