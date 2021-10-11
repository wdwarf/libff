/*
 * ff_config.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_FF_CONFIG_H_
#define FF_FF_CONFIG_H_

#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>

#define NS_FF ff
#define NS_FF_BEG namespace ff {
#define NS_FF_END }
#define USE_NS_FF using namespace ff;

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#endif

#ifndef LIBFF_API
#ifdef _WIN32

#ifdef __MINGW32__
#define LIBFF_API
#else
#ifdef LIBFF_EXPORTS
#define LIBFF_API __declspec(dllexport)
#elif defined(BUILD_SHARED_LIBS)
#define LIBFF_API __declspec(dllimport)
#else
#define LIBFF_API 
#endif
#endif

#else
#define LIBFF_API
#endif
#endif


#ifdef UNICODE

#define _W(var) w##var

#ifndef _T
#define _T(x)	L##x
#endif

#else

#define _W(var) var

#ifndef _T
#define _T(x)	x
#endif

#endif

using StringT = std::string;
using SStreamT = std::stringstream;

//#define __USE_SQLITE3_DB__

#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

/**
 * @brief 结构体定义宏
 * 
 */
#define STRUCT_DEF_BEGIN(structName) class LIBFF_API structName{\
  public:\
  structName(){memset(this, 0, sizeof(structName));}

#define STRUCT_DEF_END };

#define MEMBER_DEF_NUM_(name, type, funcOut, funcIn) public:\
  type name() const{ return funcOut(this->m_##name); }\
  void name(const type val){ this->m_##name = funcIn(val); }\
  private:\
  type m_##name

#define MEMBER_DEF_I8(name) public:\
  int8_t name() const{ return this->m_##name; }\
  void name(const int8_t val){ this->m_##name = val; }\
  private:\
  int8_t m_##name

#define MEMBER_DEF_I16(name) MEMBER_DEF_NUM_(name, int16_t, ntohs, htons)
#define MEMBER_DEF_I32(name) MEMBER_DEF_NUM_(name, int32_t, ntohl, htonl)
#define MEMBER_DEF_I64(name) MEMBER_DEF_NUM_(name, int64_t, ntohll, htonll)

#define MEMBER_DEF_U8(name) public:\
  uint8_t name() const{ return this->m_##name; }\
  void name(const uint8_t val){ this->m_##name = val; }\
  private:\
  uint8_t m_##name

#define MEMBER_DEF_U16(name) MEMBER_DEF_NUM_(name, uint16_t, ntohs, htons)
#define MEMBER_DEF_U32(name) MEMBER_DEF_NUM_(name, uint32_t, ntohl, htonl)
#define MEMBER_DEF_U64(name) MEMBER_DEF_NUM_(name, uint64_t, ntohll, htonll)

#define MEMBER_DEF_STR(name, size) public:\
  const char* name() const { return this->m_##name; }\
  void name(const char* val) { snprintf(this->m_##name, sizeof(this->m_##name), "%s", val); }\
  void name(const std::string& val) { snprintf(this->m_##name, sizeof(this->m_##name), "%s", val.c_str()); }\
  private:\
  char m_##name[size]

#endif /* FF_FF_CONFIG_H_ */
