/**
 * @file NamedPipeServer.h
 * @author liyawu
 * @date 2023-09-11 16:58:42
 * @description
 */

#ifndef _NAMEDPIPESERVER_H
#define _NAMEDPIPESERVER_H

#include <ff/ff_config.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "IOCP.h"

NS_FF_BEG

class LIBFF_API NamedPipeClient {
 public:
  NamedPipeClient(HANDLE handle, IOCP* iocp);
  ~NamedPipeClient();
  bool send(const void* buf, uint32_t len);
  void close();

  HANDLE handle() const { return m_handle; }
  ULONG clientProcessId() const { return m_clientProcessId; }

 private:
  HANDLE m_handle;
  IOCP* m_iocp;
  ULONG m_clientProcessId;
};

using NamedPipeClientPtr = std::shared_ptr<NamedPipeClient>;

struct NPClientContext : public IocpContext {
  NamedPipeClientPtr client;
};

using OnConnectedFunc = std::function<void(const NamedPipeClientPtr& client)>;
using OnDisconnectedFunc = OnConnectedFunc;
using OnDataFunc = std::function<void(const NamedPipeClientPtr& client,
                                      const void* buf, uint32_t len)>;

class LIBFF_API NamedPipeServer {
 public:
  NamedPipeServer();
  ~NamedPipeServer();

  bool start(const std::string& pipeName, OnConnectedFunc connectedFunc,
             OnDisconnectedFunc disconnectedFunc, OnDataFunc onDataFunc);
  void stop();

  enum class OPEvent { Connect, Read, Write, Close };

  struct OP : public OVERLAPPED {
    OPEvent event;
    std::vector<uint8_t> buf;

    OP() { memset(this, 0, sizeof(OVERLAPPED)); }

    void reset() { memset(this, 0, sizeof(OVERLAPPED)); }
  };

 private:
  std::string m_pipeName;
  OnConnectedFunc m_connectedFunc;
  OnDisconnectedFunc m_disconnectedFunc;
  OnDataFunc m_onDataFunc;
  std::thread m_acceptThread;
  std::atomic_bool m_stoped;
  HANDLE m_connEvent;
  IOCP m_iocp;
  std::mutex m_mutexCtx;
  std::set<NPClientContext*> m_contexts;
  void OnIocpEvent(DWORD numberOfBytesTransferred, ULONG_PTR completionKey,
                   LPOVERLAPPED lpOverlapped);

  enum class ConnStatus {
    Success,
    Timeout,
    Failed,
  };
  ConnStatus waitForConnection(HANDLE handle);
};

NS_FF_END

#endif  // _NAMEDPIPESERVER_H
