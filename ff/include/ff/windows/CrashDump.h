/**
 * @file CrashDump.h
 * @author liyawu
 * @date 2024-07-08 17:19:44
 * @description
 */

#ifndef _CRASHDUMP_H
#define _CRASHDUMP_H

#include <ff/ff_config.h>
#include <functional>

NS_FF_BEG

using CrashDumpHandler = std::function<void(const std::string& dumpStr)>;
LIBFF_API void CrashDumpInit(CrashDumpHandler handler);

NS_FF_END

#endif  // _CRASHDUMP_H
