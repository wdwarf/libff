#ifndef _FF_STRINGENCODING_H_
#define _FF_STRINGENCODING_H_

#include <ff/ff_config.h>

#include <string>

NS_FF_BEG

/** 十六进制字符转数字 */
LIBFF_API unsigned int HexAToI(char x);
LIBFF_API char IToHexA(unsigned int x);

enum class Encode : uint16_t { ANSI = 1, UTF16_LE, UTF16_BE, UTF8_BOM, UTF8 };

LIBFF_API Encode EncodeOf(const std::string& str);
LIBFF_API Encode EncodeOf(const char* str, size_t size);

LIBFF_API std::string Utf8ToGbk(const std::string& str);
LIBFF_API std::string GbkToUtf8(const std::string& str);
LIBFF_API std::wstring ToWs(const std::string& str);
LIBFF_API std::string ToMbs(const std::wstring& str);

NS_FF_END

#endif