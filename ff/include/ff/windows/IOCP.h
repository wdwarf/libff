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

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

NS_FF_BEG

using IocpEventFunc =
    std::function<void(DWORD numberOfBytesTransferred, ULONG_PTR completionKey,
                       LPOVERLAPPED lpOverlapped)>;

struct LIBFF_API IocpContext {
  HANDLE handle;
  IocpEventFunc eventFunc;
};

using PIocpContext = IocpContext*;

class LIBFF_API IOCP {
 public:
  IOCP(DWORD numberOfConcurrentThreads = 0);
  ~IOCP();

  void close();
  bool create(DWORD numberOfConcurrentThreads = 0);
  bool connect(PIocpContext context);
  operator HANDLE() const;
  operator bool() const;
  uint16_t activeWorkThreadCnt() const;
  bool getQueuedCompletionStatus(LPDWORD lpNumberOfBytesTransferred,
                                 PULONG_PTR lpCompletionKey,
                                 LPOVERLAPPED* lpOverlapped,
                                 DWORD dwMilliseconds);

  bool postQueuedCompletionStatus(DWORD numberOfBytesTransferred,
                                  ULONG_PTR completionKey,
                                  LPOVERLAPPED lpOverlapped);

 private:
  HANDLE m_handle;
  std::vector<std::thread> m_workThreads;
  std::atomic_uint16_t m_activeWorkThreadCnt;
};

using IOCPPtr = std::shared_ptr<IOCP>;
using GIocp = Singleton<IOCP>;

NS_FF_END

#endif /* FF_IOCP_H_ */
