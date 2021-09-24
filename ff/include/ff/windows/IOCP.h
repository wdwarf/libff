/*
 * IOCP.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef FF_IOCP_H_
#define FF_IOCP_H_

#include <ff/Singleton.h>
#include <ff/ff_config.h>

#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

NS_FF_BEG

enum class IocpEvent { Accept, Recv, Send, Close };

class IocpContext : public OVERLAPPED {
 public:
  IocpContext();
  IocpContext(HANDLE handle, IocpEvent event);

  HANDLE handle;
  IocpEvent iocpEevent;
  WSABUF buffer;
};

using PIocpContext = IocpContext*;

using IocpWorkThreadFunc =
    std::function<void(LPDWORD lpNumberOfBytesTransferred,
                       PULONG_PTR lpCompletionKey, LPOVERLAPPED* lpOverlapped)>;

class IOCP {
 public:
  IOCP(DWORD concurrentThreads = 8);
  ~IOCP();

  void close();
  bool create(DWORD numberOfConcurrentThreads);
  bool connect(HANDLE fileHandle, ULONG_PTR completionKey,
               IocpWorkThreadFunc iocpWorkThreadFunc);
  bool disconnect(HANDLE fileHandle);
  operator HANDLE() const;
  operator bool() const;
  bool getQueuedCompletionStatus(LPDWORD lpNumberOfBytesTransferred,
                                 PULONG_PTR lpCompletionKey,
                                 LPOVERLAPPED* lpOverlapped,
                                 DWORD dwMilliseconds);

  bool postQueuedCompletionStatus(DWORD numberOfBytesTransferred,
                                  ULONG_PTR completionKey,
                                  LPOVERLAPPED lpOverlapped);

 private:
  HANDLE m_handle;
  std::mutex m_mutex;
  std::vector<std::thread> m_workThreads;
  std::map<HANDLE, IocpWorkThreadFunc> m_iocpWorkThreadFuncs;
};

using IOCPPtr = std::shared_ptr<IOCP>;
using GIocp = Singleton<IOCP>;

NS_FF_END

#endif /* FF_IOCP_H_ */
