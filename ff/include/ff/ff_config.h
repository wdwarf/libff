/*
 * ff_config.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_FF_CONFIG_H_
#define FF_FF_CONFIG_H_

#include <cstdint>

#define NS_FF ff


#ifndef LIBFF_API
#ifdef _WIN32

#ifdef LIBFF_EXPORTS
#define LIBFF_API __declspec(dllexport)
#else
#define LIBFF_API __declspec(dllimport)
#endif

#else
#define LIBFF_API
#endif
#endif


namespace NS_FF {

typedef unsigned char boolean;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

}

#define __USE_SQLITE3_DB__

#endif /* FF_FF_CONFIG_H_ */
