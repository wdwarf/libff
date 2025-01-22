/**
 * @file Utility.cpp
 * @author liyawu
 * @date 2024-12-17 10:57:25
 * @description
 */
#include <ff/Exception.h>
#include <ff/String.h>
#include <ff/Utility.h>

#include <iostream>

NS_FF_BEG

void ff_assert(const char *assertion, const char *file, int line) noexcept {
  std::cerr << "ASSERT: \"" << assertion << "\" in file " << file << ", line "
            << line << std::endl;
}

NS_FF_END
