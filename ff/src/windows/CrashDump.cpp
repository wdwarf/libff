/**
 * @file CrashDump.cpp
 * @author liyawu
 * @date 2024-07-08 17:18:44
 * @description
 */
#include <ff/String.h>
#include <ff/windows/CrashDump.h>

#if 1  // Make the file formater not to change the position of this line
#include <windows.h>
#endif

#include <dbghelp.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Rpcrt4.lib")

using namespace std;

NS_FF_BEG

namespace {
CrashDumpHandler g_crashDumpHandler;

struct SymbolInfo {
  DWORD64 address = 0;
  std::string moduleName;
  std::string functionName;
  std::string fileName;
  int lineNumber = 0;

  static std::string GetFileName(const std::string &filePath) {
    auto pos = filePath.find_last_of('\\');
    if (std::string::npos == pos) return filePath;
    return filePath.substr(pos + 1);
  }
};

std::vector<SymbolInfo> ObtainStackTrace(LPEXCEPTION_POINTERS info) {
  DWORD machine_type{IMAGE_FILE_MACHINE_I386};
  STACKFRAME64 frame{};
  frame.AddrPC.Mode = AddrModeFlat;
  frame.AddrFrame.Mode = AddrModeFlat;
  frame.AddrStack.Mode = AddrModeFlat;

  CONTEXT *context = info->ContextRecord;
#ifdef _M_IX86
  frame.AddrPC.Offset = context->Eip;
  frame.AddrFrame.Offset = context->Ebp;
  frame.AddrStack.Offset = context->Esp;
  machine_type = IMAGE_FILE_MACHINE_I386;
#elif _M_X64
  frame.AddrPC.Offset = context->Rip;
  frame.AddrFrame.Offset = context->Rbp;
  frame.AddrStack.Offset = context->Rsp;
  machine_type = IMAGE_FILE_MACHINE_AMD64;
#elif _M_IA64
  frame.AddrPC.Offset = context->StIIP;
  frame.AddrFrame.Offset = context->IntSp;
  frame.AddrStack.Offset = context->IntSp;
  machine_type = IMAGE_FILE_MACHINE_IA64;
  frame.AddrBStore.Offset = context.RsBSP;
  frame.AddrBStore.Mode = AddrModeFlat;
#else
  frame.AddrPC.Offset = context->Eip;
  frame.AddrFrame.Offset = context->Ebp;
  frame.AddrStack.Offset = context->Esp;
  machine_type = IMAGE_FILE_MACHINE_I386;
#endif

  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();
  SymInitialize(process, NULL, TRUE);
  std::vector<SymbolInfo> stackTrace;
  while (StackWalk64(machine_type, process, thread, &frame, context, NULL,
                     SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
    if (0 == frame.AddrPC.Offset) break;

    const auto bufSize = sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR);
    ULONG64 symBuf[bufSize]{0};
    PSYMBOL_INFO pSymInfo = (PSYMBOL_INFO)symBuf;
    pSymInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymInfo->MaxNameLen = MAX_SYM_NAME;
    SymbolInfo symInfo;
    symInfo.address = frame.AddrPC.Offset;

    DWORD64 moduleBase = SymGetModuleBase64(process, frame.AddrPC.Offset);
    char ModuleName[MAX_PATH]{0};
    if (moduleBase &&
        GetModuleFileNameA((HINSTANCE)moduleBase, ModuleName, MAX_PATH)) {
      symInfo.moduleName = ModuleName;
    }

    DWORD64 symDisplacement = 0;
    if (SymFromAddr(process, frame.AddrPC.Offset, &symDisplacement, pSymInfo)) {
      symInfo.functionName = pSymInfo->Name;
    }

    stackTrace.push_back(symInfo);
  }
  SymCleanup(process);
  return stackTrace;
}

string GetModuleByRetAddr(PBYTE Ret_Addr, PBYTE &Module_Addr,
                          DWORD &offsetAddr) {
  MODULEENTRY32 M{};
  M.dwSize = sizeof(M);

  TCHAR Module_Name[MAX_PATH] = {0};

  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
  if ((hSnapshot != INVALID_HANDLE_VALUE) && ::Module32First(hSnapshot, &M)) {
    do {
      if (static_cast<DWORD>(Ret_Addr - M.modBaseAddr) < M.modBaseSize) {
        offsetAddr = Ret_Addr - M.modBaseAddr;
        lstrcpyn(Module_Name, M.szExePath, MAX_PATH);
        Module_Addr = M.modBaseAddr;
        break;
      }
    } while (::Module32Next(hSnapshot, &M));
  }
  CloseHandle(hSnapshot);

#ifdef UNICODE
  return ff::ToMbs(Module_Name);
#else
  return Module_Name;
#endif
}

std::string GetCallStack(PEXCEPTION_POINTERS pException) {
  char buffer[512] = {0};
  std::stringstream str;

  auto symInfos = ObtainStackTrace(pException);
  for (auto &symInfo : symInfos) {
    DWORD offsetAddr{0};
    PBYTE Module_Addr_1{0};

    auto addr = symInfo.address;
    auto moduleName =
        GetModuleByRetAddr((PBYTE)addr, Module_Addr_1, offsetAddr);

    memset(buffer, 0, sizeof(0));
#ifdef _WIN64
    sprintf(buffer, "\n\t%016llX(:%08lX)", addr, offsetAddr);
#else
    sprintf(buffer, "\n\t%08lX(:%08lX)", static_cast<DWORD>(addr), offsetAddr);
#endif
    str << (buffer);

    if (!symInfo.moduleName.empty()) {
      str << "  ";
      str << symInfo.moduleName;
      if (!symInfo.functionName.empty())
        str << "(" << symInfo.functionName << ")";
    } else
      str << "<unknown>";
  }

  return str.str();
}

std::string GetOSVersion(DWORD majorVersion, DWORD minorVersion,
                         DWORD buildNumber, BYTE productType) {
  switch (majorVersion) {
    case 5:
      switch (minorVersion) {
        case 0:
          return "Windows 2000";
        case 1:
          return "Windows XP";
        case 2:
          if (productType == VER_NT_WORKSTATION) {
            return "Windows XP Professional x64 Edition";
          } else {
            if (GetSystemMetrics(SM_SERVERR2)) {
              return "Windows Server 2003 R2";
            } else {
              return "Windows Server 2003";
            }
          }
        default:
          return {};
      }
    case 6:
      switch (minorVersion) {
        case 0:
          if (productType == VER_NT_WORKSTATION) {
            return "Windows Vista";
          } else {
            return "Windows Server 2008";
          }
        case 1:
          if (productType == VER_NT_WORKSTATION) {
            return "Windows 7";
          } else {
            return "Windows Server 2008 R2";
          }
        case 2:
          if (productType == VER_NT_WORKSTATION) {
            return "Windows 8";
          } else {
            return "Windows Server 2012";
          }
        case 3:
          if (productType == VER_NT_WORKSTATION) {
            return "Windows 8.1";
          } else {
            return "Windows Server 2012 R2";
          }
        default:
          return {};
      }
    case 10:
      if (productType == VER_NT_WORKSTATION) {
        if (buildNumber < 22000) {
          return "Windows 10";
        } else {
          return "Windows 11";
        }
      } else {
        if (buildNumber < 17763) {
          return "Windows Server 2016";
        } else if (buildNumber < 22000) {
          return "Windows Server 2019";
        } else {
          return "Windows Server 2022";
        }
      }
    default:
      return {};
  }
}

std::string GetVersionStr() {
  OSVERSIONINFOEX V{};  // EX for NT 5.0 and later
  V.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  if (!GetVersionEx(reinterpret_cast<POSVERSIONINFO>(&V))) {
    ZeroMemory(&V, sizeof(V));
    V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(reinterpret_cast<POSVERSIONINFO>(&V));
  }

  if (V.dwPlatformId != VER_PLATFORM_WIN32_NT) {
    V.dwBuildNumber =
        LOWORD(V.dwBuildNumber);  // for 9x HIWORD(dwBuildNumber) = 0x04xx
  }

  std::string sRet = "OS: " + GetOSVersion(V.dwMajorVersion, V.dwMinorVersion,
                                           V.dwBuildNumber, V.wProductType);
#ifdef UNICODE
  std::string sp = ff::ToMbs(V.szCSDVersion);
#else
  std::string sp = V.szCSDVersion;
#endif
  if (!sp.empty()) {
    sRet += ", " + sp;
  }
  return sRet;
}

std::string GetWorkingSet() {
  PROCESS_MEMORY_COUNTERS pmc{};
  pmc.cb = sizeof pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof pmc);
  std::string str = "Current " +
                    std::to_string(pmc.WorkingSetSize / 1024 / 1024) +
                    "MB   Peak " +
                    std::to_string(pmc.PeakWorkingSetSize / 1024 / 1024) + "MB";
  return str;
}

std::string GetVirtualSize() {
  MEMORYSTATUSEX memoryStatusEx{};
  memoryStatusEx.dwLength = sizeof memoryStatusEx;
  GlobalMemoryStatusEx(&memoryStatusEx);
  std::string str = std::to_string((memoryStatusEx.ullTotalVirtual -
                                    memoryStatusEx.ullAvailVirtual) /
                                   1024 / 1024) +
                    "MB";
  return str;
}

std::string applicationName() {
  char cPath[1024] = {0};
  GetModuleFileNameA(NULL, cPath, 1024);
  std::string path = cPath;
  auto pos = path.find_last_of('\\');
  return path.substr(pos + 1);
}

std::string getOsArchStr() {
#ifdef _WIN64
  return "64 bit";
#else
  return "32 bit";
#endif
}

std::string GetFileVersionStr(const std::string &strFile) {
  std::string ver;
  char szVersionBuffer[1000] = {0};
  DWORD dwVerSize;
  DWORD dwHandle;

  dwVerSize = GetFileVersionInfoSizeA(strFile.c_str(), &dwHandle);
  if (dwVerSize == 0) return ver;

  if (GetFileVersionInfoA(strFile.c_str(), 0, dwVerSize, szVersionBuffer)) {
    VS_FIXEDFILEINFO *pInfo;
    unsigned int nInfoLen;

    if (VerQueryValueA(szVersionBuffer, ("\\"), (void **)&pInfo, &nInfoLen)) {
      std::stringstream str;
      str << HIWORD(pInfo->dwFileVersionMS) << (".")
          << LOWORD(pInfo->dwFileVersionMS) << "."
          << HIWORD(pInfo->dwFileVersionLS) << (".")
          << LOWORD(pInfo->dwFileVersionLS);
      ver = str.str();
    }
  }

  return ver;
}

std::string getAppVersion() {
  char name[MAX_PATH];
  GetModuleFileNameA(NULL, name, MAX_PATH);
  return GetFileVersionStr(name);
}

std::string getBuildDateTime() { return ""; }

std::string GetCrashInfo(PEXCEPTION_POINTERS pException) {
  std::string workingSet = GetWorkingSet();
  std::string virtualSize = GetVirtualSize();

  char Module_Name[MAX_PATH];
  GetModuleFileNameA(NULL, Module_Name, MAX_PATH);

  std::stringstream str;
  str << applicationName() << "(" << getOsArchStr() << ") " << getAppVersion()
      << ", built " << getBuildDateTime() << "\n\n";

  str << (GetVersionStr());

  str << ("\nProcess:  ");
  str << Module_Name;

  // If exception occurred.
  if (pException) {
    EXCEPTION_RECORD &E = *pException->ExceptionRecord;
    CONTEXT &C = *pException->ContextRecord;

    // If module with E.ExceptionAddress found - save its path and date.
    DWORD offsetAddr{0};
    PBYTE Module_Addr{0};
    std::string module = GetModuleByRetAddr(
        reinterpret_cast<PBYTE>(E.ExceptionAddress), Module_Addr, offsetAddr);
    char buffer[512] = {0};
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "\n\nException Addr:  %p(:%08lX)  ", E.ExceptionAddress,
            offsetAddr);
    str << (buffer);
    if (!module.empty()) {
      str << (module);
    }

    switch (E.ExceptionCode) {
      case EXCEPTION_ACCESS_VIOLATION: {
        sprintf(buffer,
                "\nException Code:  ACCESS_VIOLATION(%08lX)\n\t%s Address:  %p",
                E.ExceptionCode, (E.ExceptionInformation[0]) ? "Write" : "Read",
                reinterpret_cast<PBYTE>(E.ExceptionInformation[1]));
      } break;
      case EXCEPTION_DATATYPE_MISALIGNMENT: {
        sprintf(buffer, "\nException Code:  DATATYPE_MISALIGNMENT(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_BREAKPOINT: {
        sprintf(buffer, "\nException Code:  BREAKPOINT(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_SINGLE_STEP: {
        sprintf(buffer, "\nException Code:  SINGLE_STEP(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: {
        sprintf(buffer, "\nException Code:  ARRAY_BOUNDS_EXCEEDED(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_DENORMAL_OPERAND: {
        sprintf(buffer, "\nException Code:  FLT_DENORMAL_OPERAND(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO: {
        sprintf(buffer, "\nException Code:  FLT_DEVICE_BY_ZERO(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_INEXACT_RESULT: {
        sprintf(buffer, "\nException Code:  FLT_INEXACT_RESULT(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_INVALID_OPERATION: {
        sprintf(buffer, "\nException Code:  FLT_INVALID_OPERATION(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_OVERFLOW: {
        sprintf(buffer, "\nException Code:  FLT_OVERFLOW(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_STACK_CHECK: {
        sprintf(buffer, "\nException Code:  FLT_STACK_CHECK(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_FLT_UNDERFLOW: {
        sprintf(buffer, "\nException Code:  FLT_UNDERFLOW(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO: {
        sprintf(buffer, "\nException Code:  INT_DIVIDE_BY_ZERO(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_INT_OVERFLOW: {
        sprintf(buffer, "\nException Code:  INT_OVERFLOW(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_PRIV_INSTRUCTION: {
        sprintf(buffer, "\nException Code:  PRIV_INSTRUCTION(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_IN_PAGE_ERROR: {
        sprintf(buffer, "\nException Code:  IN_PAGE_ERROR(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_ILLEGAL_INSTRUCTION: {
        sprintf(buffer, "\nException Code:  ILLEGAL_INSTRUCTION(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_NONCONTINUABLE_EXCEPTION: {
        sprintf(buffer, "\nException Code:  NONCONTINUABLE_EXCEPTION(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_STACK_OVERFLOW: {
        sprintf(buffer, "\nException Code:  STACK_OVERFLOW(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_INVALID_DISPOSITION: {
        sprintf(buffer, "\nException Code:  INVALID_DISPOSITION(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_GUARD_PAGE: {
        sprintf(buffer, "\nException Code:  GUARD_PAGE(%08lX)",
                E.ExceptionCode);
      } break;
      case EXCEPTION_INVALID_HANDLE: {
        sprintf(buffer, "\nException Code:  INVALID_HANDLE(%08lX)",
                E.ExceptionCode);
      } break;
      default: {
        sprintf(buffer, "\nException Code:  %08lX", E.ExceptionCode);
      } break;
    }
    str << (buffer);

    str << ("\nInstruction: ");
    for (int i = 0; i < 16; i++) {
      sprintf(buffer, " %02X", reinterpret_cast<PBYTE>(E.ExceptionAddress)[i]);
      str << (buffer);
    }

    str << ("\nRegisters: ");

#ifdef _WIN64
    sprintf(buffer,
            "\n\tRAX: %016llX  RBX: %016llX  RCX: %016llX  RDX: %016llX", C.Rax,
            C.Rbx, C.Rcx, C.Rdx);
    str << (buffer);
    sprintf(buffer,
            "\n\tRSI: %016llX  RDI: %016llX  RSP: %016llX  RBP: %016llX", C.Rsi,
            C.Rdi, C.Rsp, C.Rbp);
    str << (buffer);
    sprintf(buffer,
            "\n\tR8:  %016llX  R9:  %016llX  R10: %016llX  R11: %016llX", C.R8,
            C.R9, C.R10, C.R11);
    str << (buffer);
    sprintf(buffer,
            "\n\tR12: %016llX  R13: %016llX  R14: %016llX  R15: %016llX", C.R12,
            C.R13, C.R14, C.R15);
    str << (buffer);
    sprintf(buffer, "\n\tRIP: %016llX  EFlags: %08lX", C.Rip, C.EFlags);
    str << (buffer);
#else
    sprintf(buffer, "\n\tEAX: %08lX  EBX: %08lX  ECX: %08lX  EDX: %08lX", C.Eax,
            C.Ebx, C.Ecx, C.Edx);
    str << (buffer);
    sprintf(buffer, "\n\tESI: %08lX  EDI: %08lX  ESP: %08lX  EBP: %08lX", C.Esi,
            C.Edi, C.Esp, C.Ebp);
    str << (buffer);
    sprintf(buffer, "\n\tEIP: %08lX  EFlags: %08lX", C.Eip, C.EFlags);
    str << (buffer);
#endif
  }  // if (pException)

  str << ("\n\nMemory:\n\tWorking Set: ");
  str << (workingSet);
  str << ("\n\tVirtual Size: ");
  str << (virtualSize);

  str << ("\n\nCall Stack:");
  str << (GetCallStack(pException));

  return str.str();
}

long __stdcall _UnhandledExceptionFilter(_EXCEPTION_POINTERS *excep) {
  auto crashInfo = GetCrashInfo(excep);
  if (g_crashDumpHandler) {
    g_crashDumpHandler(crashInfo);
  } else
    cout << crashInfo << endl;

  return EXCEPTION_EXECUTE_HANDLER;
}

void _InvalidParameterHandler(const wchar_t * /*expression*/,
                              const wchar_t * /*function*/,
                              const wchar_t * /*file*/, unsigned int /*line*/,
                              uintptr_t /*pReserved*/) {
  RaiseException(0xFF0000, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

void _Terminate() {
  RaiseException(0xFF0001, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

}  // namespace

LIBFF_API void CrashDumpInit(CrashDumpHandler handler) {
  g_crashDumpHandler = handler;

  SetUnhandledExceptionFilter(_UnhandledExceptionFilter);
  // CRT
  _set_invalid_parameter_handler(_InvalidParameterHandler);
  // C++ exception
  std::set_terminate(_Terminate);
}

NS_FF_END
