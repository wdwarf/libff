/**
 * @file System.cpp
 * @auth DuckyLi
 * @date 2022-02-28 11:14:52
 * @description
 */
#include <ff/System.h>

#include <array>
#include <bitset>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

using namespace std;

NS_FF_BEG

#ifdef __linux__
#define __cpuid(pInfo, func)                                           \
  asm volatile("cpuid"                                                 \
               : "=a"((pInfo)[0]), "=b"((pInfo)[1]), "=c"((pInfo)[2]), \
                 "=d"((pInfo)[3])                                      \
               : "a"((func)))

// ecx is often an input as well as an output
#define __cpuid4(func, a, b, c, d) \
  asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(func))

#endif

class __CpuInfo {
  class __CpuInfoImpl;

 public:
  // getters
  static std::string Vendor() { return impl.vendor_; }
  static std::string Brand() { return impl.brand_; }
  static std::string CpuId() { return impl.cpuId_; }

  static bool SSE3() { return impl.f_1_ECX_[0]; }
  static bool PCLMULQDQ() { return impl.f_1_ECX_[1]; }
  static bool MONITOR() { return impl.f_1_ECX_[3]; }
  static bool SSSE3() { return impl.f_1_ECX_[9]; }
  static bool FMA() { return impl.f_1_ECX_[12]; }
  static bool CMPXCHG16B() { return impl.f_1_ECX_[13]; }
  static bool SSE41() { return impl.f_1_ECX_[19]; }
  static bool SSE42() { return impl.f_1_ECX_[20]; }
  static bool MOVBE() { return impl.f_1_ECX_[22]; }
  static bool POPCNT() { return impl.f_1_ECX_[23]; }
  static bool AES() { return impl.f_1_ECX_[25]; }
  static bool XSAVE() { return impl.f_1_ECX_[26]; }
  static bool OSXSAVE() { return impl.f_1_ECX_[27]; }
  static bool AVX() { return impl.f_1_ECX_[28]; }
  static bool F16C() { return impl.f_1_ECX_[29]; }
  static bool RDRAND() { return impl.f_1_ECX_[30]; }

  static bool MSR() { return impl.f_1_EDX_[5]; }
  static bool CX8() { return impl.f_1_EDX_[8]; }
  static bool SEP() { return impl.f_1_EDX_[11]; }
  static bool CMOV() { return impl.f_1_EDX_[15]; }
  static bool CLFSH() { return impl.f_1_EDX_[19]; }
  static bool MMX() { return impl.f_1_EDX_[23]; }
  static bool FXSR() { return impl.f_1_EDX_[24]; }
  static bool SSE() { return impl.f_1_EDX_[25]; }
  static bool SSE2() { return impl.f_1_EDX_[26]; }

  static bool FSGSBASE() { return impl.f_7_EBX_[0]; }
  static bool BMI1() { return impl.f_7_EBX_[3]; }
  static bool HLE() { return impl.isIntel_ && impl.f_7_EBX_[4]; }
  static bool AVX2() { return impl.f_7_EBX_[5]; }
  static bool BMI2() { return impl.f_7_EBX_[8]; }
  static bool ERMS() { return impl.f_7_EBX_[9]; }
  static bool INVPCID() { return impl.f_7_EBX_[10]; }
  static bool RTM() { return impl.isIntel_ && impl.f_7_EBX_[11]; }
  static bool AVX512F() { return impl.f_7_EBX_[16]; }
  static bool RDSEED() { return impl.f_7_EBX_[18]; }
  static bool ADX() { return impl.f_7_EBX_[19]; }
  static bool AVX512PF() { return impl.f_7_EBX_[26]; }
  static bool AVX512ER() { return impl.f_7_EBX_[27]; }
  static bool AVX512CD() { return impl.f_7_EBX_[28]; }
  static bool SHA() { return impl.f_7_EBX_[29]; }

  static bool PREFETCHWT1() { return impl.f_7_ECX_[0]; }

  static bool LAHF() { return impl.f_81_ECX_[0]; }
  static bool LZCNT() { return impl.isIntel_ && impl.f_81_ECX_[5]; }
  static bool ABM() { return impl.isAMD_ && impl.f_81_ECX_[5]; }
  static bool SSE4a() { return impl.isAMD_ && impl.f_81_ECX_[6]; }
  static bool XOP() { return impl.isAMD_ && impl.f_81_ECX_[11]; }
  static bool TBM() { return impl.isAMD_ && impl.f_81_ECX_[21]; }

  static bool SYSCALL() { return impl.isIntel_ && impl.f_81_EDX_[11]; }
  static bool MMXEXT() { return impl.isAMD_ && impl.f_81_EDX_[22]; }
  static bool RDTSCP() { return impl.isIntel_ && impl.f_81_EDX_[27]; }
  static bool _3DNOWEXT() { return impl.isAMD_ && impl.f_81_EDX_[30]; }
  static bool _3DNOW() { return impl.isAMD_ && impl.f_81_EDX_[31]; }

 private:
  static const __CpuInfoImpl impl;

  class __CpuInfoImpl {
   public:
    __CpuInfoImpl()
        : nIds_{0},
          nExIds_{0},
          isIntel_{false},
          isAMD_{false},
          f_1_ECX_{0},
          f_1_EDX_{0},
          f_7_EBX_{0},
          f_7_ECX_{0},
          f_81_ECX_{0},
          f_81_EDX_{0},
          data_{},
          extdata_{} {
      std::array<int, 4> cpui;

      // Calling __cpuid with 0x0 as the function_id argument
      // gets the number of the highest valid function ID.
      __cpuid(cpui.data(), 0);
      nIds_ = cpui[0];

      for (int i = 0; i <= nIds_; ++i) {
        __cpuid(cpui.data(), i);
        data_.push_back(cpui);
      }

      // Capture vendor string
      char vendor[0x20] = {0};
      *reinterpret_cast<int *>(vendor) = data_[0][1];
      *reinterpret_cast<int *>(vendor + 4) = data_[0][3];
      *reinterpret_cast<int *>(vendor + 8) = data_[0][2];
      vendor_ = vendor;
      if (vendor_ == "GenuineIntel") {
        isIntel_ = true;
      } else if (vendor_ == "AuthenticAMD") {
        isAMD_ = true;
      }

      // load bitset with flags for function 0x00000001
      if (nIds_ >= 1) {
        f_1_ECX_ = data_[1][2];
        f_1_EDX_ = data_[1][3];

        stringstream str;
        str << hex;
        str.fill('0');
        str.width(8);
        str << std::uppercase << data_[1][3];
        str.fill('0');
        str.width(8);
        str << std::uppercase << data_[1][0];

        this->cpuId_ = str.str();
      }

      // load bitset with flags for function 0x00000007
      if (nIds_ >= 7) {
        f_7_EBX_ = data_[7][1];
        f_7_ECX_ = data_[7][2];
      }

      // Calling __cpuid with 0x80000000 as the function_id argument
      // gets the number of the highest valid extended ID.
      __cpuid(cpui.data(), 0x80000000);
      nExIds_ = cpui[0];

      char brand[0x40];
      memset(brand, 0, sizeof(brand));

      for (int i = 0x80000000; i <= nExIds_; ++i) {
        __cpuid(cpui.data(), i);
        extdata_.push_back(cpui);
      }

      // load bitset with flags for function 0x80000001
      if (nExIds_ >= 0x80000001) {
        f_81_ECX_ = extdata_[1][2];
        f_81_EDX_ = extdata_[1][3];
      }

      // Interpret CPU brand string if reported
      if (nExIds_ >= 0x80000004) {
        memcpy(brand, extdata_[2].data(), sizeof(cpui));
        memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
        memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
        brand_ = brand;
      }
    };

    int nIds_;
    int nExIds_;
    std::string cpuId_;
    std::string vendor_;
    std::string brand_;
    bool isIntel_;
    bool isAMD_;
    std::bitset<32> f_1_ECX_;
    std::bitset<32> f_1_EDX_;
    std::bitset<32> f_7_EBX_;
    std::bitset<32> f_7_ECX_;
    std::bitset<32> f_81_ECX_;
    std::bitset<32> f_81_EDX_;
    std::vector<std::array<int, 4>> data_;
    std::vector<std::array<int, 4>> extdata_;
  };
};

const __CpuInfo::__CpuInfoImpl __CpuInfo::impl;

std::string CpuInfo::cpuId() { return __CpuInfo::CpuId(); }
std::string CpuInfo::vendor() { return __CpuInfo::Vendor(); }
std::string CpuInfo::brand() { return __CpuInfo::Brand(); }

bool CpuInfo::SSE3() { return __CpuInfo::SSE3(); }
bool CpuInfo::PCLMULQDQ() { return __CpuInfo::PCLMULQDQ(); }
bool CpuInfo::MONITOR() { return __CpuInfo::MONITOR(); }
bool CpuInfo::SSSE3() { return __CpuInfo::SSSE3(); }
bool CpuInfo::FMA() { return __CpuInfo::FMA(); }
bool CpuInfo::CMPXCHG16B() { return __CpuInfo::CMPXCHG16B(); }
bool CpuInfo::SSE41() { return __CpuInfo::SSE41(); }
bool CpuInfo::SSE42() { return __CpuInfo::SSE42(); }
bool CpuInfo::MOVBE() { return __CpuInfo::MOVBE(); }
bool CpuInfo::POPCNT() { return __CpuInfo::POPCNT(); }
bool CpuInfo::AES() { return __CpuInfo::AES(); }
bool CpuInfo::XSAVE() { return __CpuInfo::XSAVE(); }
bool CpuInfo::OSXSAVE() { return __CpuInfo::OSXSAVE(); }
bool CpuInfo::AVX() { return __CpuInfo::AVX(); }
bool CpuInfo::F16C() { return __CpuInfo::F16C(); }
bool CpuInfo::RDRAND() { return __CpuInfo::RDRAND(); }

bool CpuInfo::MSR() { return __CpuInfo::MSR(); }
bool CpuInfo::CX8() { return __CpuInfo::CX8(); }
bool CpuInfo::SEP() { return __CpuInfo::SEP(); }
bool CpuInfo::CMOV() { return __CpuInfo::CMOV(); }
bool CpuInfo::CLFSH() { return __CpuInfo::CLFSH(); }
bool CpuInfo::MMX() { return __CpuInfo::MMX(); }
bool CpuInfo::FXSR() { return __CpuInfo::FXSR(); }
bool CpuInfo::SSE() { return __CpuInfo::SSE(); }
bool CpuInfo::SSE2() { return __CpuInfo::SSE2(); }

bool CpuInfo::FSGSBASE() { return __CpuInfo::FSGSBASE(); }
bool CpuInfo::BMI1() { return __CpuInfo::BMI1(); }
bool CpuInfo::HLE() { return __CpuInfo::HLE(); }
bool CpuInfo::AVX2() { return __CpuInfo::AVX2(); }
bool CpuInfo::BMI2() { return __CpuInfo::BMI2(); }
bool CpuInfo::ERMS() { return __CpuInfo::ERMS(); }
bool CpuInfo::INVPCID() { return __CpuInfo::INVPCID(); }
bool CpuInfo::RTM() { return __CpuInfo::RTM(); }
bool CpuInfo::AVX512F() { return __CpuInfo::AVX512F(); }
bool CpuInfo::RDSEED() { return __CpuInfo::RDSEED(); }
bool CpuInfo::ADX() { return __CpuInfo::ADX(); }
bool CpuInfo::AVX512PF() { return __CpuInfo::AVX512PF(); }
bool CpuInfo::AVX512ER() { return __CpuInfo::AVX512ER(); }
bool CpuInfo::AVX512CD() { return __CpuInfo::AVX512CD(); }
bool CpuInfo::SHA() { return __CpuInfo::SHA(); }

bool CpuInfo::PREFETCHWT1() { return __CpuInfo::PREFETCHWT1(); }

bool CpuInfo::LAHF() { return __CpuInfo::LAHF(); }
bool CpuInfo::LZCNT() { return __CpuInfo::LZCNT(); }
bool CpuInfo::ABM() { return __CpuInfo::ABM(); }
bool CpuInfo::SSE4a() { return __CpuInfo::SSE4a(); }
bool CpuInfo::XOP() { return __CpuInfo::XOP(); }
bool CpuInfo::TBM() { return __CpuInfo::TBM(); }

bool CpuInfo::SYSCALL() { return __CpuInfo::SYSCALL(); }
bool CpuInfo::MMXEXT() { return __CpuInfo::MMXEXT(); }
bool CpuInfo::RDTSCP() { return __CpuInfo::RDTSCP(); }
bool CpuInfo::_3DNOWEXT() { return __CpuInfo::_3DNOWEXT(); }
bool CpuInfo::_3DNOW() { return __CpuInfo::_3DNOW(); }

System::System(/* args */) {}

System::~System() {}

std::string System::Hostname() {
  char buf[128] = {0};
  gethostname(buf, 128);
  return buf;
}

ff::MemoryStatus System::MemoryStatus() {
  ff::MemoryStatus info;
  memset(&info, 0, sizeof(ff::MemoryStatus));

#ifdef _WIN32
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&status);

  info.phyTotal = status.ullTotalPhys;
  info.phyFree = status.ullAvailPhys;
  info.phyUsed = info.phyTotal - info.phyFree;
  info.swapTotal = status.ullTotalVirtual;
  info.swapFree = status.ullAvailVirtual;
  info.swapUsed = info.swapTotal - info.swapFree;
#else
  struct sysinfo status;
  if (0 != sysinfo(&status)) return info;

  info.phyTotal = status.totalram;
  info.phyFree = status.freeram;
  info.phyUsed = info.phyTotal - info.phyFree;
  info.swapTotal = status.totalswap;
  info.swapFree = status.freeswap;
  info.swapUsed = info.swapTotal - info.swapFree;
#endif

  return info;
}

CpuInfo System::CpuInfo() { return ff::CpuInfo(); }

NS_FF_END
