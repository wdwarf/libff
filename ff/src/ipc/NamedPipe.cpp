/**
 * @file NamedPipe.cpp
 * @author DuckyLi
 * @date 2023-08-10 08:06:39
 * @description
 */
#include <fcntl.h>
#include <ff/File.h>
#include <ff/NamedPipe.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef _WIN32
#include <sys/select.h>
#include <unistd.h>
#endif

NS_FF_BEG

NamedPipe::NamedPipe() {
#ifdef _WIN32
  m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  m_hReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
}

NamedPipe::~NamedPipe() {
  this->close();
#ifdef _WIN32
  CloseHandle(m_hReadEvent);
  CloseHandle(m_hWriteEvent);
#endif
}

bool NamedPipe::create(const std::string& pipeName) {
  this->close();

  m_pipeName = pipeName;

#ifdef _WIN32
  m_handle = CreateNamedPipeA(
      m_pipeName.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
      PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, 1024 * 1024, 1024 * 1024,
      NMPWAIT_WAIT_FOREVER, NULL);
  if (INVALID_HANDLE_VALUE == m_handle) {
    return false;
  }

  return true;
#else
  File pipeFile(pipeName);
  if (pipeFile.isExists()) {
    struct stat64 buf;
    if (0 != stat64(pipeName.c_str(), &buf)) return false;

    if (S_ISFIFO(buf.st_mode)) {
      return true;
    }
  }

  m_pipeFd = mkfifo(pipeName.c_str(), 0777);
  return (m_pipeFd >= 0);
#endif
}

bool NamedPipe::open(const std::string& pipeName, uint32_t timeoutMs) {
  // this->close();

  m_pipeName = pipeName;

#ifdef _WIN32
  if (INVALID_HANDLE_VALUE != m_handle) {
    return true;
  }

  if (!WaitNamedPipeA(m_pipeName.c_str(), timeoutMs)) {
    return false;
  }

  m_handle = CreateFileA(m_pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                         NULL, OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
  if (INVALID_HANDLE_VALUE == m_handle) {
    return false;
  }

  DWORD mode = PIPE_READMODE_MESSAGE;
  if (!SetNamedPipeHandleState(m_handle, &mode, NULL, NULL)) {
    return false;
  }

  return true;

#else
  if (m_pipeFd >= 0) return true;

  m_pipeFd = ::open(pipeName.c_str(), O_NONBLOCK | O_RDWR);
  return (m_pipeFd >= 0);
#endif
}

void NamedPipe::close() {
#ifdef _WIN32
  if (INVALID_HANDLE_VALUE == m_handle) return;

  DisconnectNamedPipe(m_handle);
  CloseHandle(m_handle);
  m_handle = INVALID_HANDLE_VALUE;

#else
  if (m_pipeFd < 0) return;
  ::close(m_pipeFd);
  m_pipeFd = -1;

  unlink(m_pipeName.c_str());
#endif
}

bool NamedPipe::connect(uint32_t timeoutMs) {
#ifdef _WIN32
  if (INVALID_HANDLE_VALUE == m_handle) return false;

  OVERLAPPED op;
  ZeroMemory(&op, sizeof(OVERLAPPED));
  op.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

  if (ConnectNamedPipe(m_handle, &op) ||
      ERROR_PIPE_CONNECTED == GetLastError()) {
    CloseHandle(op.hEvent);
    return true;
  }

  if (WAIT_OBJECT_0 != WaitForSingleObject(op.hEvent, timeoutMs)) {
    CloseHandle(op.hEvent);
    return false;
  }

  CloseHandle(op.hEvent);
  return true;
#else
  return (m_pipeFd >= 0);
#endif
}

int NamedPipe::write(const void* buf, int bufSize, uint32_t timeoutMs) {
#ifdef _WIN32
  if (INVALID_HANDLE_VALUE == m_handle) return -1;

  OVERLAPPED op;
  ZeroMemory(&op, sizeof(OVERLAPPED));
  op.hEvent = m_hWriteEvent;

  DWORD writeBytes = 0;
  if (FALSE == WriteFile(m_handle, buf, bufSize, &writeBytes, &op)) {
    if (ERROR_IO_PENDING != GetLastError()) {
      return -1;
    }

    auto waitRt = WaitForSingleObject(m_hWriteEvent, timeoutMs);
    if (WAIT_OBJECT_0 != waitRt) {
      CancelIoEx(m_handle, &op);
      return -1;
    }

    if (!GetOverlappedResult(m_handle, &op, &writeBytes, FALSE)) return -1;
    FlushFileBuffers(m_handle);
  }

  return writeBytes;
#else
  if (m_pipeFd < 0) return -1;
  fd_set fs_read;
  timeval tv;
  tv.tv_sec = timeoutMs / 1000;
  tv.tv_usec = (timeoutMs % 1000) * 1000;
  FD_ZERO(&fs_read);
  FD_SET(this->m_pipeFd, &fs_read);
  int re =
      ::select(this->m_pipeFd + 1, 0, &fs_read, 0, (-1 == timeoutMs ? 0 : &tv));
  if (re <= 0) {
    return -1;
  }
  return ::write(this->m_pipeFd, buf, bufSize);
#endif
}

int NamedPipe::read(void* buf, int bufSize, uint32_t timeoutMs) {
#ifdef _WIN32
  if (INVALID_HANDLE_VALUE == m_handle) return -1;

  OVERLAPPED op;
  ZeroMemory(&op, sizeof(OVERLAPPED));
  op.hEvent = m_hReadEvent;

  DWORD readBytes = 0;
  if (FALSE == ReadFile(m_handle, buf, bufSize, &readBytes, &op)) {
    if (ERROR_MORE_DATA == GetLastError()) return bufSize;

    if (ERROR_IO_PENDING != GetLastError()) {
      return -1;
    }

    auto waitRt = WaitForSingleObject(m_hReadEvent, timeoutMs);
    if (WAIT_OBJECT_0 != waitRt) {
      CancelIoEx(m_handle, &op);
      return -1;
    }

    if (!GetOverlappedResult(m_handle, &op, &readBytes, FALSE)) {
      if (ERROR_MORE_DATA == GetLastError()) return bufSize;
      return -1;
    }
  }

  return readBytes;
#else
  if (m_pipeFd < 0) return -1;
  fd_set fs_read;
  timeval tv;
  tv.tv_sec = timeoutMs / 1000;
  tv.tv_usec = (timeoutMs % 1000) * 1000;
  FD_ZERO(&fs_read);
  FD_SET(this->m_pipeFd, &fs_read);
  int re =
      ::select(this->m_pipeFd + 1, &fs_read, 0, 0, (-1 == timeoutMs ? 0 : &tv));
  if (re <= 0) {
    return -1;
  }
  return ::read(this->m_pipeFd, buf, bufSize);
#endif
}

std::string NamedPipe::pipeName() const { return m_pipeName; }

NS_FF_END
