/**
 * @file System.h
 * @auth DuckyLi
 * @date 2022-02-28 11:14:29
 * @description
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <ff/ff_config.h>

#include <string>

NS_FF_BEG

struct LIBFF_API MemoryStatus {
  uint64_t phyTotal;
  uint64_t phyFree;
  uint64_t phyUsed;
  uint64_t swapTotal;
  uint64_t swapUsed;
  uint64_t swapFree;
};

class LIBFF_API CpuInfo {
 public:
  std::string cpuId();
  std::string vendor();
  std::string brand();
  uint32_t clock();
  uint32_t cores();
  uint32_t cacheSize();
  uint32_t family();
  uint32_t extendedFamily();
  uint32_t model();
  uint32_t extendedModel();
  uint32_t steppingId();

  bool SSE3();
  bool PCLMULQDQ();
  bool MONITOR();
  bool SSSE3();
  bool FMA();
  bool CMPXCHG16B();
  bool SSE41();
  bool SSE42();
  bool MOVBE();
  bool POPCNT();
  bool AES();
  bool XSAVE();
  bool OSXSAVE();
  bool AVX();
  bool F16C();
  bool RDRAND();

  bool MSR();
  bool CX8();
  bool SEP();
  bool CMOV();
  bool CLFSH();
  bool MMX();
  bool FXSR();
  bool SSE();
  bool SSE2();

  bool FSGSBASE();
  bool BMI1();
  bool HLE();
  bool AVX2();
  bool BMI2();
  bool ERMS();
  bool INVPCID();
  bool RTM();
  bool AVX512F();
  bool RDSEED();
  bool ADX();
  bool AVX512PF();
  bool AVX512ER();
  bool AVX512CD();
  bool SHA();

  bool PREFETCHWT1();

  bool LAHF();
  bool LZCNT();
  bool ABM();
  bool SSE4a();
  bool XOP();
  bool TBM();

  bool SYSCALL();
  bool MMXEXT();
  bool RDTSCP();
  bool _3DNOWEXT();
  bool _3DNOW();
};

class LIBFF_API System {
 private:
  /* data */
  System(/* args */);
  ~System();

 public:
  static std::string Hostname();
  static ff::MemoryStatus MemoryStatus();
  static ff::CpuInfo CpuInfo();
};

NS_FF_END

#endif
