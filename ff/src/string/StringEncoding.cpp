#include <ff/Exception.h>
#include <ff/StringEncoding.h>

#include <cstring>
#include <vector>

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

Encode Utf8EncodeDetect(const char* str, size_t length) {
  const uint8_t* data = (const uint8_t*)str;
  uint32_t nBytes = 0;  // UFT8可用1-6个字节编码,ASCII用一个字节
  uint8_t chr = 0;
  bool bAllAscii = true;  // 如果全部都是ASCII, 说明不是UTF-8
  for (size_t i = 0; i < length; ++i) {
    chr = data[i];

    // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
    if ((chr & 0x80) != 0) bAllAscii = false;
    if (0 == nBytes)  // 如果不是ASCII码,应该是多字节符,计算字节数
    {
      // is ASCII
      if (chr < 0x80) continue;

      if (chr >= 0xFC && chr <= 0xFD)
        nBytes = 6;
      else if (chr >= 0xF8)
        nBytes = 5;
      else if (chr >= 0xF0)
        nBytes = 4;
      else if (chr >= 0xE0)
        nBytes = 3;
      else if (chr >= 0xC0)
        nBytes = 2;
      else
        return Encode::ANSI;

      nBytes--;

    } else {  // 多字节符的非首字节,应为 10xxxxxx
      if ((chr & 0xC0) != 0x80) return Encode::ANSI;

      nBytes--;
    }
  }

  if (nBytes > 0)  // 违返规则
    return Encode::ANSI;

  if (bAllAscii)  // 如果全部都是ASCII, 说明不是UTF-8
    return Encode::ANSI;

  return Encode::UTF8;
}

Encode Utf8EncodeDetect(const std::string& str) {
  return Utf8EncodeDetect(str.c_str(), str.length());
}

Encode EncodeOf(const char* str, size_t size) {
  const uint8_t* data = (const uint8_t*)str;

  if (size > 2 && data[0] == 0xFF && data[1] == 0xFE) {
    return Encode::UTF16_LE;
  }

  if (size > 2 && data[0] == 0xFE && data[1] == 0xFF) {
    return Encode::UTF16_BE;
  }

  if (size > 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
    return Encode::UTF8_BOM;
  }

  return Utf8EncodeDetect(str, size);
}

Encode EncodeOf(const std::string& str) {
  return EncodeOf(str.c_str(), str.length());
}

#ifdef WIN32

std::string Utf8ToGbk(const std::string& srcStr) {
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

NS_FF_END
