/*
 * ProcessImplWindows.cpp
 *
 *  Created on: 2018年7月2日
 *      Author: admin
 */
#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__)

#include "ProcessImplWindows.h"

#include <Tlhelp32.h>
#include <ff/String.h>

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>

using namespace std;

NS_FF_BEG

namespace {

typedef UINT PROCESSINFOCLASS;
typedef NTSTATUS(NTAPI* F_NtQueryInformationProcess)(
    HANDLE ProcessHandle, PROCESSINFOCLASS InformationClass,
    PVOID ProcessInformation, ULONG ProcessInformationLength,
    PULONG ReturnLength OPTIONAL);

typedef struct {
  DWORD ExitStatus;
  DWORD PebBaseAddress;
  DWORD AffinityMask;
  DWORD BasePriority;
  ULONG UniqueProcessId;
  ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

}  // namespace

Process::ProcessImpl::ProcessImpl(Process* proc, const std::string& command)
    : _proc(proc), asyncRead(true), m_exitCode(-1) {
  this->command = command;
  memset(&this->pi, 0, sizeof(PROCESS_INFORMATION));
  this->pi.hProcess = INVALID_HANDLE_VALUE;
  m_readPipe[0] = INVALID_HANDLE_VALUE;
  m_readPipe[1] = INVALID_HANDLE_VALUE;
  m_writePipe[0] = INVALID_HANDLE_VALUE;
  m_writePipe[1] = INVALID_HANDLE_VALUE;
}

Process::ProcessImpl::~ProcessImpl() {
  try {
    this->stop();
  } catch (...) {
  }
}

void Process::ProcessImpl::start() {
  this->stop();

  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;
  if (!::CreatePipe(&this->m_readPipe[0], &this->m_readPipe[1], &sa, 0)) {
    THROW_EXCEPTION(ProcessException,
                    "process start failed: CreatePipe failed.", GetLastError());
  }

  SetHandleInformation(m_readPipe[0], HANDLE_FLAG_INHERIT, 0);

  if (!::CreatePipe(&this->m_writePipe[1], &this->m_writePipe[0], &sa, 0)) {
    THROW_EXCEPTION(ProcessException,
                    "process start failed: CreatePipe failed.", GetLastError());
  }

  SetHandleInformation(m_writePipe[0], HANDLE_FLAG_INHERIT, 0);

  std::string cmdLine;
  for (size_t i = 0; i < args.size(); ++i) {
    cmdLine += " " + args[i];
  }

  LPSTR pAppName = NULL;
  LPSTR pCmdLine = (LPSTR)this->command.c_str();
  if (!cmdLine.empty()) {
    pAppName = (LPSTR)this->command.c_str();
    pCmdLine = (LPSTR)cmdLine.c_str();
  }

  STARTUPINFOA si;
  si.cb = sizeof(STARTUPINFOA);
  GetStartupInfoA(&si);
  si.hStdError = m_readPipe[1];
  si.hStdOutput = m_readPipe[1];
  si.hStdInput = m_writePipe[1];
  si.wShowWindow = SW_HIDE;  // SW_SHOW;
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  if (!::CreateProcessA(pAppName, pCmdLine, NULL, NULL, TRUE, 0, NULL,
                        this->workDir.empty() ? NULL : this->workDir.c_str(),
                        &si, &this->pi)) {
    THROW_EXCEPTION(ProcessException,
                    "process start failed: CreateProcess[" + command +
                        "] failed, errcode(" + to_string(GetLastError()) + ").",
                    GetLastError());
  }

  CloseHandle(m_readPipe[1]);
  m_readPipe[1] = INVALID_HANDLE_VALUE;
  CloseHandle(m_writePipe[1]);
  m_writePipe[1] = INVALID_HANDLE_VALUE;

  if (this->asyncRead) {
    this->readThread = thread(&Process::ProcessImpl::doReadData, this);
  }

  this->watchThread = thread(&Process::ProcessImpl::watchTerminated, this);
}

void Process::ProcessImpl::stop() {
  if (INVALID_HANDLE_VALUE == this->pi.hProcess) return;

  ::TerminateProcess(this->pi.hProcess, 0);

  if (this->watchThread.joinable()) this->watchThread.join();

  if (this->readThread.joinable()) this->readThread.join();

  if (INVALID_HANDLE_VALUE != this->m_readPipe[0]) {
    CloseHandle(this->m_readPipe[0]);
    this->m_readPipe[0] = INVALID_HANDLE_VALUE;
  }

  if (INVALID_HANDLE_VALUE != this->m_readPipe[1]) {
    CloseHandle(this->m_readPipe[1]);
    this->m_readPipe[1] = INVALID_HANDLE_VALUE;
  }

  if (INVALID_HANDLE_VALUE != this->m_writePipe[0]) {
    CloseHandle(this->m_writePipe[0]);
    this->m_writePipe[0] = INVALID_HANDLE_VALUE;
  }

  if (INVALID_HANDLE_VALUE != this->m_writePipe[1]) {
    CloseHandle(this->m_writePipe[1]);
    this->m_writePipe[1] = INVALID_HANDLE_VALUE;
  }

  CloseHandle(this->pi.hProcess);
  memset(&this->pi, 0, sizeof(PROCESS_INFORMATION));
  this->pi.hProcess = INVALID_HANDLE_VALUE;
}

int Process::ProcessImpl::getExitCode() const { return this->m_exitCode; }

void Process::ProcessImpl::watchTerminated() { this->waitForFinished(); }

int Process::ProcessImpl::waitForFinished() {
  if (INVALID_HANDLE_VALUE == this->pi.hProcess) return -1;
  ::WaitForSingleObject(this->pi.hProcess, INFINITE);
  DWORD exitCode = -1;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  this->m_exitCode = exitCode;
  return this->m_exitCode;
}

int Process::ProcessImpl::getProcessId() const { return this->pi.dwProcessId; }

int Process::ProcessImpl::readData(char* buf, int bufLen) {
  if (INVALID_HANDLE_VALUE == this->pi.hProcess ||
      INVALID_HANDLE_VALUE == this->m_readPipe[0] || this->asyncRead)
    return -1;

  DWORD readBytes = 0;
  if (ReadFile(this->m_readPipe[0], buf, bufLen, &readBytes, NULL)) {
    return readBytes;
  }
  return -1;
}

int Process::ProcessImpl::writeData(const char* buf, int bufLen) {
  if (INVALID_HANDLE_VALUE == this->pi.hProcess ||
      INVALID_HANDLE_VALUE == this->m_writePipe[0])
    return -1;

  DWORD readBytes = 0;
  if (WriteFile(this->m_writePipe[0], buf, bufLen, &readBytes, NULL)) {
    return readBytes;
  }
  return -1;
}

void Process::ProcessImpl::doReadData() {
  if (INVALID_HANDLE_VALUE == this->pi.hProcess) return;

  const DWORD bufLen = 2048;
  char buf[bufLen];
  DWORD readBytes = 0;
  while (ReadFile(this->m_readPipe[0], buf, bufLen, &readBytes, NULL)) {
    if (readBytes > 0) {
      this->_proc->onReadData(buf, readBytes);
    } else {
      break;
    }
  }
}

int Process::ProcessImpl::GetPid() { return ::GetProcessId(NULL); }

int Process::ProcessImpl::GetPPid() {
  int pid = Process::ProcessImpl::GetPid();
  PROCESS_BASIC_INFORMATION pbi = {0};
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
  if (INVALID_HANDLE_VALUE == hProcess) return -1;
  F_NtQueryInformationProcess fNtQueryInformationProcess =
      (F_NtQueryInformationProcess)GetProcAddress(GetModuleHandle("ntdll"),
                                                  "NtQueryInformationProcess");
  fNtQueryInformationProcess(hProcess, 0, &pbi,
                             sizeof(PROCESS_BASIC_INFORMATION), NULL);
  CloseHandle(hProcess);
  return pbi.InheritedFromUniqueProcessId;
}

int Process::ProcessImpl::Exec(const std::string& command, bool wait) {
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  si.cb = sizeof(STARTUPINFO);
  GetStartupInfo(&si);
  si.hStdError = NULL;
  si.hStdOutput = NULL;
  si.wShowWindow = SW_SHOW;
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  if (!::CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 0, NULL,
                       NULL, &si, &pi)) {
    THROW_EXCEPTION(ProcessException, "process start failed.", GetLastError());
  }

  if (wait) {
    ::WaitForSingleObject(pi.hProcess, INFINITE);
  }

  return pi.dwProcessId;
}

int Process::ProcessImpl::GetPidByName(const std::string& processName) {
  string procName = ToLowerCopy(processName);
  HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hProcSnap) return -1;

  PROCESSENTRY32 pe;
  pe.dwSize = sizeof(PROCESSENTRY32);
  int pid = -1;
  if (Process32First(hProcSnap, &pe)) {
    do {
      string name = ToLowerCopy(pe.szExeFile);
      if (name == procName) {
        pid = pe.th32ProcessID;
        break;
      }
    } while (Process32Next(hProcSnap, &pe));
  }

  CloseHandle(hProcSnap);
  return pid;
}

bool Process::ProcessImpl::Kill(int pid, int code) {
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (INVALID_HANDLE_VALUE == hProcess) return false;
  BOOL re = ::TerminateProcess(hProcess, code);
  CloseHandle(hProcess);
  return (TRUE == re);
}

bool Process::ProcessImpl::Kill(const std::string& processName, int code) {
  int pid = Process::GetPidByName(processName);
  if (pid <= 0) return false;
  return Process::ProcessImpl::Kill(pid, code);
}

std::map<int, std::string> Process::ProcessImpl::ListProcesses() {
  std::map<int, std::string> re;

  HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hProcSnap) return re;

  PROCESSENTRY32 pe;
  pe.dwSize = sizeof(PROCESSENTRY32);
  int pid = -1;
  if (Process32First(hProcSnap, &pe)) {
    do {
      re.insert(make_pair(pe.th32ProcessID, pe.szExeFile));
    } while (Process32Next(hProcSnap, &pe));
  }

  CloseHandle(hProcSnap);
  return re;
}

const std::string& Process::ProcessImpl::getCommand() const { return command; }

void Process::ProcessImpl::setCommand(const std::string& command) {
  this->command = command;
}

bool Process::ProcessImpl::isAsyncRead() const { return asyncRead; }

void Process::ProcessImpl::setAsyncRead(bool asyncRead) {
  if (INVALID_HANDLE_VALUE != this->pi.hProcess) {
    THROW_EXCEPTION(ProcessException, "process is running.", -1);
  }
  this->asyncRead = asyncRead;
}

const std::string& Process::ProcessImpl::getWorkDir() const { return workDir; }

void Process::ProcessImpl::setWorkDir(const std::string& workDir) {
  this->workDir = workDir;
}

void Process::ProcessImpl::addParameter(const std::string& arg) {
  this->args.push_back(arg);
}

const vector<string>& Process::ProcessImpl::getParameters() const {
  return this->args;
}

void Process::ProcessImpl::clearParameter() { this->args.clear(); }

NS_FF_END

#endif
