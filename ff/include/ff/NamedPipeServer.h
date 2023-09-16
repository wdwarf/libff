/**
 * @file NamedPipeServer.h
 * @author DuckyLi
 * @date 2023-09-13 17:03:17
 * @description
 */

#ifndef _NAMEDPIPESERVER_H_
#define _NAMEDPIPESERVER_H_

#ifdef _WIN32

#include <ff/ff_config.h>
#include <ff/windows/IOCP.h>
#include <string>
#include <thread>

NS_FF_BEG

class LIBFF_API NamedPipeServer {
 public:
  NamedPipeServer();
  ~NamedPipeServer();

  bool start(const std::string& pipeName);
  void stop();

 private:
  std::string m_pipeName;
  IOCP m_iocp;
  HANDLE m_connectEvent;
  std::thread m_acceptThread;
};

NS_FF_END

#endif

#endif /** _NAMEDPIPESERVER_H_ */
