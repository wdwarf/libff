/*
 * ff_config.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_FF_CONFIG_H_
#define FF_FF_CONFIG_H_

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#define NS_FF ff
#define NS_FF_BEG namespace ff {
#define NS_FF_END }
#define USE_NS_FF using namespace ff;

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4819)

#define ThreadLocalVal __declspec(thread)

#else

#define ThreadLocalVal __thread

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
#define _T(x) L##x
#endif

#else

#define _W(var) var

#ifndef _T
#define _T(x) x
#endif

#endif

// #define __USE_SQLITE3_DB__

#ifndef htonll

#define htonll(x)  \
  ((1 == htonl(1)) \
       ? (x)       \
       : ((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x)  \
  ((1 == ntohl(1)) \
       ? (x)       \
       : ((uint64_t)ntohl((x)&0xFFFFFFFF) << 32) | ntohl((x) >> 32))

#endif

NS_FF_BEG

using StringT = std::string;
using SStreamT = std::stringstream;

template <typename T, typename U>
void* bit_copy(T& dst, U& src) {
  static_assert(sizeof(T) == sizeof(U),
                "the parameters dst and src mast has the same size!");
  return memcpy(&dst, &src, sizeof(U));
}

NS_FF_END

/**
 * @brief 结构体定义宏
 *
 */
#define STRUCT_DEF_BEGIN(structName)                      \
  class LIBFF_API structName {                            \
   public:                                                \
    structName() { memset(this, 0, sizeof(structName)); } \
    static size_t Size() { return sizeof(structName); }

#define STRUCT_DEF_END \
  }                    \
  ;

#define MEMBER_DEF_NUM_(name, type, funcOut, funcIn)          \
 public:                                                      \
  type name() const { return funcOut(this->m_##name); }       \
  void name(const type val) { this->m_##name = funcIn(val); } \
                                                              \
 private:                                                     \
  type m_##name

#define MEMBER_DEF_I8(name)                             \
 public:                                                \
  int8_t name() const { return this->m_##name; }        \
  void name(const int8_t val) { this->m_##name = val; } \
                                                        \
 private:                                               \
  int8_t m_##name

#define MEMBER_DEF_I16(name) MEMBER_DEF_NUM_(name, int16_t, ntohs, htons)
#define MEMBER_DEF_I32(name) MEMBER_DEF_NUM_(name, int32_t, ntohl, htonl)
#define MEMBER_DEF_I64(name) MEMBER_DEF_NUM_(name, int64_t, ntohll, htonll)

#define MEMBER_DEF_U8(name)                              \
 public:                                                 \
  uint8_t name() const { return this->m_##name; }        \
  void name(const uint8_t val) { this->m_##name = val; } \
                                                         \
 private:                                                \
  uint8_t m_##name

#define MEMBER_DEF_U16(name) MEMBER_DEF_NUM_(name, uint16_t, ntohs, htons)
#define MEMBER_DEF_U32(name) MEMBER_DEF_NUM_(name, uint32_t, ntohl, htonl)
#define MEMBER_DEF_U64(name) MEMBER_DEF_NUM_(name, uint64_t, ntohll, htonll)

#define MEMBER_DEF_FLOAT(name)          \
 public:                                \
  float name() const {                  \
    float ret;                          \
    uint32_t v = ntohl(this->m_##name); \
    ff::bit_copy(ret, v);               \
    return ret;                         \
  }                                     \
  void name(const float val) {          \
    uint32_t v;                         \
    ff::bit_copy(v, val);               \
    this->m_##name = htonl(v);          \
  }                                     \
                                        \
 private:                               \
  uint32_t m_##name

#define MEMBER_DEF_DOUBLE(name)          \
 public:                                 \
  double name() const {                  \
    double ret;                          \
    uint64_t v = ntohll(this->m_##name); \
    ff::bit_copy(ret, v);                \
    return ret;                          \
  }                                      \
  void name(const double val) {          \
    uint64_t v;                          \
    ff::bit_copy(v, val);                \
    this->m_##name = htonll(v);          \
  }                                      \
                                         \
 private:                                \
  uint64_t m_##name

#define MEMBER_DEF_STR(name, size)                    \
 public:                                              \
  const char* name() const { return this->m_##name; } \
  void name(const char* val) {                        \
    strncpy(this->m_##name, val, size);               \
    this->m_##name[size - 1] = '\0';                  \
  }                                                   \
  void name(const std::string& val) {                 \
    strncpy(this->m_##name, val.c_str(), size);       \
    this->m_##name[size - 1] = '\0';                  \
  }                                                   \
                                                      \
 private:                                             \
  char m_##name[size]

#define MEMBER_DEF_BYTEARR(name, size)                                        \
 public:                                                                      \
  uint8_t(&name())[size] { return this->m_##name; }                           \
  const uint8_t(&name() const)[size] { return this->m_##name; }               \
  void name(const uint8_t(&val)[size]) { memcpy(this->m_##name, val, size); } \
                                                                              \
 private:                                                                     \
  uint8_t m_##name[size]

NS_FF_BEG

extern void ff_assert(const char* assertion, const char* file,
                      int line) noexcept;

NS_FF_END

#if !defined(FF_ASSERT)
#define FF_ASSERT(cond) \
  ((cond) ? static_cast<void>(0) : ff::ff_assert(#cond, __FILE__, __LINE__))
#endif

#endif /* FF_FF_CONFIG_H_ */
