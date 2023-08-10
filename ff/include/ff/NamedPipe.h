/**
 * @file NamedPipe.h
 * @author DuckyLi
 * @date 2023-08-10 08:07:17
 * @description
 */

#ifndef _FF_NAMEDPIPE_H_
#define _FF_NAMEDPIPE_H_

#include <ff/ff_config.h>

#include <string>

NS_FF_BEG

class NamedPipe {
 public:
  NamedPipe(/* args */);
  ~NamedPipe();

  bool create(const std::string& pipeName);
  bool connect(uint32_t timeoutMs = 2000);

  bool open(const std::string& pipeName, uint32_t timeoutMs = 2000);

  int write(const void* buf, int bufSize, uint32_t timeoutMs = 2000);
  int read(void* buf, int bufSize, uint32_t timeoutMs = 2000);

  void close();

  std::string pipeName() const;

 private:
  std::string m_pipeName;

#ifdef _WIN32
  HANDLE m_handle = INVALID_HANDLE_VALUE;
  HANDLE m_hWriteEvent = INVALID_HANDLE_VALUE;
  HANDLE m_hReadEvent = INVALID_HANDLE_VALUE;
#else
  int m_pipeFd;
#endif
};

NS_FF_END

#endif /** _FF_NAMEDPIPE_H_ */
