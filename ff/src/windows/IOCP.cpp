/*
 * IOCP.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#include <ff/windows/IOCP.h>

#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

NS_FF_BEG

IOCP::IOCP(DWORD numberOfConcurrentThreads)
    : m_handle(NULL), m_activeWorkThreadCnt(0) {
  if (!this->create(numberOfConcurrentThreads)) {
    cerr << "failed to create iocp port" << endl;
  }
}

IOCP::~IOCP() { this->close(); }

IOCP::operator HANDLE() const { return this->m_handle; }

IOCP::operator bool() const { return (NULL != this->m_handle); }

uint16_t IOCP::activeWorkThreadCnt() const {
  return this->m_activeWorkThreadCnt.load();
}

bool IOCP::create(DWORD numberOfConcurrentThreads) {
  if (-1 == numberOfConcurrentThreads) return false;

  this->close();

  this->m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL,
                                          numberOfConcurrentThreads);
  if ((NULL == this->m_handle)) return false;

  if (0 == numberOfConcurrentThreads) {
    SYSTEM_INFO SysemInfo;
    GetSystemInfo(&SysemInfo);
    numberOfConcurrentThreads = SysemInfo.dwNumberOfProcessors + 2;
  }

  this->m_workThreads = std::vector<std::thread>(numberOfConcurrentThreads);
  for (DWORD i = 0; i < numberOfConcurrentThreads; ++i) {
    this->m_workThreads[i] = thread([this] {
      ++this->m_activeWorkThreadCnt;
      while (true) {
        DWORD numberOfBytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED lpOverlapped = nullptr;

        if (!this->getQueuedCompletionStatus(&numberOfBytesTransferred,
                                             &completionKey, &lpOverlapped,
                                             INFINITE)) {
          continue;
        }

        if ((0 == completionKey && nullptr == lpOverlapped)) break;

        PIocpContext context = (PIocpContext)completionKey;
        assert(nullptr != context);
        if (!context->eventFunc) {
          continue;
        }

        context->eventFunc(numberOfBytesTransferred, completionKey,
                           lpOverlapped);
      }
      --this->m_activeWorkThreadCnt;
    });
  }

  return (NULL != this->m_handle);
}

bool IOCP::connect(PIocpContext context) {
  return (this->m_handle == CreateIoCompletionPort(context->handle,
                                                   this->m_handle,
                                                   (ULONG_PTR)context, 0));
}

void IOCP::close() {
  if (NULL == this->m_handle) return;

  for (auto& t : this->m_workThreads) {
    PostQueuedCompletionStatus(this->m_handle, 0, 0, nullptr);
  };

  for (auto& t : this->m_workThreads) {
    t.join();
  };

  CloseHandle(this->m_handle);
  this->m_handle = NULL;
}

bool IOCP::getQueuedCompletionStatus(__out LPDWORD lpNumberOfBytesTransferred,
                                     __out PULONG_PTR lpCompletionKey,
                                     __out LPOVERLAPPED* lpOverlapped,
                                     __in DWORD dwMilliseconds) {
  return (TRUE == ::GetQueuedCompletionStatus(
                      this->m_handle, lpNumberOfBytesTransferred,
                      lpCompletionKey, lpOverlapped, dwMilliseconds));
}

bool IOCP::postQueuedCompletionStatus(DWORD numberOfBytesTransferred,
                                      ULONG_PTR completionKey,
                                      LPOVERLAPPED lpOverlapped) {
  return (TRUE == PostQueuedCompletionStatus(this->m_handle,
                                             numberOfBytesTransferred,
                                             completionKey, lpOverlapped));
}

NS_FF_END