/**
 * @file NamedPipe.cpp
 * @author DuckyLi
 * @date 2023-08-10 08:06:39
 * @description
 */
#include <ff/NamedPipe.h>

NS_FF_BEG

NamedPipe::NamedPipe(/* args */) {}

NamedPipe::~NamedPipe() {}

bool NamedPipe::create(const std::string& pipeName) {
  this->close();

  m_pipeName = pipeName;

#ifdef _WIN32
  m_handle = CreateNamedPipeA(m_pipeName.c_str(),
                              PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                              PIPE_TYPE_MESSAGE, 1, 1024 * 1024, 1024 * 1024,
                              NMPWAIT_WAIT_FOREVER, NULL);
  if (INVALID_HANDLE_VALUE == m_handle) {
    return false;
  }

  return true;
#else
  //
#endif
}

bool NamedPipe::open(const std::string& pipeName, uint32_t timeoutMs) {
  this->close();

  m_pipeName = pipeName;

#ifdef _WIN32
  if (!WaitNamedPipeA(m_pipeName.c_str(), timeoutMs)) {
    return false;
  }

  m_handle = CreateFileA(m_pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == m_handle) {
    return false;
  }

  return true;

#else
  //
#endif
}

void NamedPipe::close() {
#ifdef _WIN32
  if (INVALID_HANDLE_VALUE == m_handle) return;

  DisconnectNamedPipe(m_handle);
  CloseHandle(m_handle);
  m_handle = INVALID_HANDLE_VALUE;

#else
  //
#endif
}

bool NamedPipe::connect(uint32_t timeoutMs) {
#ifdef _WIN32
  if (INVALID_HANDLE_VALUE == m_handle) return false;

  OVERLAPPED op;
  ZeroMemory(&op, sizeof(OVERLAPPED));
  op.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

  if (ConnectNamedPipe(m_handle, &op)) {
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
  //
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
      return -1;
    }
    ResetEvent(m_hWriteEvent);

    GetOverlappedResult(m_handle, &op, &writeBytes, FALSE);
    FlushFileBuffers(m_handle);
  }

  return writeBytes;
#else
  //
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
    if (ERROR_IO_PENDING != GetLastError()) {
      return -1;
    }

    auto waitRt = WaitForSingleObject(m_hReadEvent, timeoutMs);
    if (WAIT_OBJECT_0 != waitRt) {
      return -1;
    }
    ResetEvent(m_hReadEvent);

    GetOverlappedResult(m_handle, &op, &readBytes, FALSE);
  }

  return readBytes;
#else
  //
#endif
}

std::string NamedPipe::pipeName() const { return m_pipeName; }

NS_FF_END
