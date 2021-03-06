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

#endif /* FF_FF_CONFIG_H_ */
