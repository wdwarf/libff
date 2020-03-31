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
#else
#define LIBFF_API __declspec(dllimport)
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

using StringT = std::_W(string);
using SStreamT = std::_W(stringstream);

//#define __USE_SQLITE3_DB__

#endif /* FF_FF_CONFIG_H_ */
