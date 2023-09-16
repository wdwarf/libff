/**
 * @file NamedPipeServer.cpp
 * @author liyawu
 * @date 2023-09-11 16:58:25
 * @description
 */
#include <ff/windows/NamedPipeServer.h>

#include <iostream>

using namespace std;
using namespace std::placeholders;

#define NP_READ_BUF_SIZE (1024)

NS_FF_BEG

NamedPipeClient::NamedPipeClient(HANDLE handle, IOCP* iocp)
    : m_handle(handle), m_iocp(iocp), m_clientProcessId(0) {
  GetNamedPipeClientProcessId(m_handle, &m_clientProcessId);
}

NamedPipeClient::~NamedPipeClient() {
  //
}

bool NamedPipeClient::send(const void* buf, uint32_t len) {
  if (INVALID_HANDLE_VALUE == m_handle) false;

  auto op = new NamedPipeServer::OP();
  op->buf.resize(len);
  memcpy(op->buf.data(), buf, len);
  op->event = NamedPipeServer::OPEvent::Write;
  auto rt =
      WriteFile(m_handle, op->buf.data(), (DWORD)op->buf.size(), NULL, op);

  if (!rt && ERROR_IO_PENDING != GetLastError()) {
    delete op;
    return false;
  }
  return true;
}

void NamedPipeClient::close() {
  if (INVALID_HANDLE_VALUE == m_handle) return;
  DisconnectNamedPipe(m_handle);
  m_handle = INVALID_HANDLE_VALUE;
}

NamedPipeServer::NamedPipeServer() {
  m_connEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
}

NamedPipeServer::~NamedPipeServer() {
  this->stop();
  CloseHandle(m_connEvent);
}

bool NamedPipeServer::start(const std::string& pipeName,
                            OnConnectedFunc connectedFunc,
                            OnDisconnectedFunc disconnectedFunc,
                            OnDataFunc onDataFunc) {
  if (m_acceptThread.joinable()) return false;

  m_stoped = false;
  m_pipeName = pipeName;
  m_connectedFunc = connectedFunc;
  m_disconnectedFunc = disconnectedFunc;
  m_onDataFunc = onDataFunc;

  m_acceptThread = std::thread([this] {
    cout << "started" << endl;
    HANDLE handle = INVALID_HANDLE_VALUE;
    while (!m_stoped) {
      if (INVALID_HANDLE_VALUE == handle) {
        handle = CreateNamedPipeA(
            m_pipeName.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES,
            1024 * 1024, 1024 * 1024, NMPWAIT_NOWAIT, NULL);
        if (INVALID_HANDLE_VALUE == handle) {
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
          continue;
        }
      }

      bool skiped = false;
      while (!m_stoped && !skiped) {
        switch (this->waitForConnection(handle)) {
          case ConnStatus::Success: {
            skiped = true;
            auto ctx = new NPClientContext();
            ctx->client = std::make_shared<NamedPipeClient>(handle, &m_iocp);
            ctx->handle = handle;
            ctx->eventFunc =
                std::bind(&NamedPipeServer::OnIocpEvent, this, _1, _2, _3);

            {
              std::lock_guard<std::mutex> lk(m_mutexCtx);
              if (m_stoped) {
                delete ctx;
                break;
              }
              m_contexts.insert(ctx);
            }
            m_iocp.connect(ctx);

            auto op = new OP();
            op->event = OPEvent::Connect;
            m_iocp.postQueuedCompletionStatus(0, (ULONG_PTR)ctx, op);

            handle = INVALID_HANDLE_VALUE;
            continue;
          } break;
          case ConnStatus::Timeout: {
            continue;
          } break;
          case ConnStatus::Failed: {
            skiped = true;
            continue;
          } break;
        }
      }

      if (INVALID_HANDLE_VALUE != handle) {
        ::CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
      }
    }

    cout << "pipe server stoped" << endl;
  });

  return true;
}

void NamedPipeServer::stop() {
  m_stoped = true;
  SetEvent(m_connEvent);
  if (m_acceptThread.joinable()) m_acceptThread.join();

  {
    std::lock_guard<std::mutex> lk(m_mutexCtx);
    for (auto ctx : m_contexts) {
      DisconnectNamedPipe(ctx->handle);
    }
  }

  while (true) {
    {
      std::lock_guard<std::mutex> lk(m_mutexCtx);
      if (m_contexts.empty()) break;
    }
    Sleep(10);
  }
}

NamedPipeServer::ConnStatus NamedPipeServer::waitForConnection(HANDLE handle) {
  if (INVALID_HANDLE_VALUE == handle) return ConnStatus::Failed;

  OVERLAPPED op;
  ZeroMemory(&op, sizeof(OVERLAPPED));
  op.hEvent = m_connEvent;

  if (ConnectNamedPipe(handle, &op) || ERROR_PIPE_CONNECTED == GetLastError()) {
    return ConnStatus::Success;
  }

  auto rt = WaitForSingleObject(op.hEvent, INFINITE);
  if (m_stoped) return ConnStatus::Failed;

  switch (rt) {
    case WAIT_TIMEOUT: {
      return ConnStatus::Timeout;
    }
    case WAIT_OBJECT_0: {
      return ConnStatus::Success;
    }
    default:
      return ConnStatus::Failed;
  }

  return ConnStatus::Failed;
}

void NamedPipeServer::OnIocpEvent(DWORD numberOfBytesTransferred,
                                  ULONG_PTR completionKey,
                                  LPOVERLAPPED lpOverlapped) {
  if (nullptr == lpOverlapped) {
    return;
  }

  auto ctx = (NPClientContext*)completionKey;
  auto op = (OP*)lpOverlapped;

  NamedPipeClientPtr client;
  {
    std::lock_guard<std::mutex> lk(m_mutexCtx);
    if (m_contexts.find(ctx) == m_contexts.end()) {
      if (nullptr != op) delete op;
      return;
    }
    client = ctx->client;
  }

  switch (op->event) {
    case OPEvent::Connect: {
      if (m_connectedFunc) m_connectedFunc(client);

      op->reset();
      op->buf.resize(NP_READ_BUF_SIZE);
      op->event = OPEvent::Read;
      auto rt = ReadFile(client->handle(), op->buf.data(),
                         (DWORD)op->buf.size(), NULL, op);
      if (rt || ERROR_IO_PENDING == GetLastError()) break;

      op->event = OPEvent::Close;
      m_iocp.postQueuedCompletionStatus(0, completionKey, op);
    } break;
    case OPEvent::Read: {
      if (numberOfBytesTransferred > 0) {
        if (m_onDataFunc)
          m_onDataFunc(client, op->buf.data(), numberOfBytesTransferred);

        op->reset();
        auto rt = ReadFile(client->handle(), op->buf.data(),
                           (DWORD)op->buf.size(), NULL, op);

        if (rt || ERROR_IO_PENDING == GetLastError()) {
          break;
        }
      }

      op->event = OPEvent::Close;
      m_iocp.postQueuedCompletionStatus(0, completionKey, op);
    } break;
    case OPEvent::Write: {
      delete op;
    } break;
    case OPEvent::Close: {
      if (m_disconnectedFunc) m_disconnectedFunc(client);

      std::lock_guard<std::mutex> lk(m_mutexCtx);
      if (m_contexts.find(ctx) == m_contexts.end()) break;
      m_contexts.erase(ctx);

      CloseHandle(ctx->handle);
      delete op;
      delete ctx;
    } break;
  }
}

NS_FF_END
