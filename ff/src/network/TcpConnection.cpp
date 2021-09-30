/*
 * TcpConnection.cpp
 *
 *  Created on: Jan 17, 2020
 *      Author: liyawu
 */

#include <errno.h>
#include <ff/Bind.h>
#include <ff/EPoll.h>
#include <ff/Exception.h>
#include <ff/StringWrapper.h>
#include <ff/TcpConnection.h>

#include <cstring>
#include <iostream>

using namespace std;

#define MAX_BUF_SIZE 8192

NS_FF_BEG

#ifdef _WIN32

enum class IocpEvent { Accept, Connected, Recv, Send, Close };

struct IoContext : public OVERLAPPED {
  IocpEvent iocpEevent;
  char buf[MAX_BUF_SIZE];
  WSABUF buffer;

  IoContext() {
    memset(this, 0, sizeof(IoContext));
    iocpEevent = IocpEvent::Recv;
    buffer.buf = this->buf;
    buffer.len = MAX_BUF_SIZE;
  }
};

// TcpConnection::TcpConnection() :
// 	m_isServer(false), m_readBuffer(MAX_BUF_SIZE) {
// 	this->m_socket.setUseSelect(false);
// }

TcpConnection::TcpConnection(IOCPPtr iocp)
    : m_isServer(false), m_readBuffer(MAX_BUF_SIZE), m_iocp(iocp) {
  this->m_socket.setUseSelect(false);
}

TcpConnection::TcpConnection(Socket&& socket, IOCPPtr iocp)
    : m_isServer(false),
      m_socket(std::move(socket)),
      m_readBuffer(MAX_BUF_SIZE) {
  this->m_socket.setUseSelect(false);
  this->m_socket.setBlocking(SockBlockingType::NonBlocking);

  this->m_iocp = iocp;
}

void TcpConnection::active() {
  this->m_iocpCtx.handle = (HANDLE)this->m_socket.getHandle();
  this->m_iocpCtx.eventFunc = Bind(&TcpConnection::workThreadFunc, this);
  if (!this->m_iocp->connect(&this->m_iocpCtx)) {
    /** FIXME how to do with this ?? */
    return;
  }

  IoContext* ioCtx = new IoContext;
  ioCtx->iocpEevent = IocpEvent::Connected;
  this->m_iocp->postQueuedCompletionStatus(0, (ULONG_PTR) & this->m_iocpCtx,
                                           ioCtx);
}

TcpConnectionPtr TcpConnection::CreateInstance(IOCPPtr iocp) {
  return TcpConnectionPtr(new TcpConnection(iocp));
}

TcpConnection::~TcpConnection() {
  this->resetCallbackFunctions();
  this->m_socket.close();
  if (this->m_isServer) {
    this->m_acceptThread.join();
  }
}

bool TcpConnection::isServer() const { return this->m_isServer; }

void TcpConnection::resetCallbackFunctions() {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onDataFunc = nullptr;
  this->m_onCloseFunc = nullptr;
  this->m_onAcceptFunc = nullptr;
  this->m_sendBuffers.clear();
}

void TcpConnection::workThreadFunc(DWORD numberOfBytesTransferred,
                                   ULONG_PTR completionKey,
                                   LPOVERLAPPED lpOverlapped) {
  PIocpContext context = (PIocpContext)completionKey;
  if (nullptr == context) return;

  IoContext* ioCtx = (IoContext*)lpOverlapped;

  auto pThis = this->shared_from_this();

  switch (ioCtx->iocpEevent) {
    case IocpEvent::Connected: {
      DWORD flags = 0;
      DWORD numToRecvd = 0;

      ioCtx->iocpEevent = IocpEvent::Recv;
      int ret = WSARecv(this->m_socket.getHandle(), &ioCtx->buffer, 1,
                        &numToRecvd, &flags, ioCtx, NULL);
      if (SOCKET_ERROR == ret) {
        auto error = WSAGetLastError();
        if (WSA_IO_PENDING != error) {
          cout << __func__ << " WSAGetLastError: " << error
               << ", socket: " << this->m_socket.getHandle() << ", "
               << this->m_socket.getRemoteAddress() << endl;

          delete ioCtx;
          this->postCloseEvent();
        }
      }
      break;
    }

    case IocpEvent::Recv: {
      if (0 == numberOfBytesTransferred) {
        delete ioCtx;
        this->postCloseEvent();
        break;
      }

      DWORD flags = 0;
      DWORD numToRecvd = 0;

      OnDataFunc func;
      {
        lock_guard<mutex> lk(this->m_mutex);
        func = this->m_onDataFunc;
      }
      if (func)
        func((const uint8_t*)ioCtx->buffer.buf, numberOfBytesTransferred,
             pThis);

      ioCtx->iocpEevent = IocpEvent::Recv;
      int ret = WSARecv(this->m_socket.getHandle(), &ioCtx->buffer, 1,
                        &numToRecvd, &flags, ioCtx, NULL);
      if (SOCKET_ERROR == ret) {
        auto error = WSAGetLastError();
        if (WSA_IO_PENDING != error) {
          cout << __func__ << " WSAGetLastError: " << error
               << ", socket: " << this->m_socket.getHandle() << ", "
               << this->m_socket.getRemoteAddress() << endl;

          delete ioCtx;
          this->postCloseEvent();
        }
      }

      break;
    }

    case IocpEvent::Send:
      delete ioCtx;
      break;

    case IocpEvent::Close: {
      delete ioCtx;

      OnCloseFunc func;
      {
        lock_guard<mutex> lk(this->m_mutex);
        func = this->m_onCloseFunc;
      }

      if (func) func(pThis);
      this->m_socket.close();
      break;
    }
  }
}

bool TcpConnection::listen(uint16_t port, const std::string& ip,
                           IpVersion ipVer, int backlog) {
  // this->resetCallbackFunctions();

  try {
    if (!this->m_socket.createTcp(ipVer))
      THROW_EXCEPTION(Exception, "Create socket failed.", errno);
    if (!this->m_socket.bind(port, ip))
      THROW_EXCEPTION(
          Exception,
          SW("Bind to ")(port)(":")(ip)(" failed. ")(strerror(errno)), errno);
    this->m_socket.listen(backlog);
  } catch (std::exception& e) {
    this->m_socket.close();
    return false;
  }

  this->m_isServer = true;

  /** TODO start accept thread */
  this->m_acceptThread = thread([this] {
    auto pThis = this->shared_from_this();
    SockAddr addr;
    while (this->m_socket.getHandle() > 0) {
      Socket client = this->m_socket.accept(addr);
      if (client.getHandle() <= 0 || INVALID_SOCKET == client.getHandle() ||
          this->m_socket.getHandle() <= 0)
        break;
      TcpConnectionPtr tcpSock =
          TcpConnectionPtr(new TcpConnection(move(client), this->m_iocp));

      OnAcceptFunc func;
      {
        lock_guard<mutex> lk(this->m_mutex);
        func = m_onAcceptFunc;
      }
      if (func) func(tcpSock);
      tcpSock->active();
    }

    if (this->m_onCloseFunc) this->m_onCloseFunc(pThis);
  });

  return true;
}

bool TcpConnection::connect(uint16_t remotePort, const std::string& remoteHost,
                            uint16_t localPort, const std::string& localIp) {
  // this->resetCallbackFunctions();

  this->m_isServer = false;
  try {
    IpVersion ipVer = IpVersion::V4;
    SockAddr addr(remoteHost, remotePort);
    if (addr.isValid()) {
      ipVer = addr.getVersion();
    }

    if (!this->m_socket.createTcp(ipVer))
      THROW_EXCEPTION(Exception, "Create socket failed.", errno);

    if (localPort > 0) {
      if (!this->m_socket.bind(localPort, localIp))
        THROW_EXCEPTION(Exception,
                        SW("Bind to ")(localPort)(
                            ":")(localIp)(" failed. ")(strerror(errno)),
                        errno);
    }

    if (!this->m_socket.connect(remoteHost, remotePort, 5000)) {
      THROW_EXCEPTION(Exception,
                      SW("Connect to ")(remoteHost)(
                          ":")(remotePort)(" failed.")(strerror(errno)),
                      errno);
    }

    this->m_socket.setBlocking(SockBlockingType::NonBlocking);

  } catch (std::exception& e) {
    this->m_socket.close();
    return false;
  }

  /** TODO add to iocp */
  this->active();

  return true;
}

void TcpConnection::close() {
  // this->m_iocp->disconnect((HANDLE)this->m_socket.getHandle());
  this->m_socket.shutdown();
  if (this->m_isServer) this->m_socket.close();
}

Socket& TcpConnection::getSocket() { return this->m_socket; }

void TcpConnection::send(const void* buf, uint32_t bufSize) {
  if (this->m_isServer) return;

  lock_guard<mutex> lk(this->m_sendMutex);

  uint32_t sendCnt = bufSize / MAX_BUF_SIZE;
  if (0 != (bufSize % MAX_BUF_SIZE)) ++sendCnt;

  const char* p = (const char*)buf;
  for (uint32_t i = 0; i < sendCnt; ++i) {
    uint32_t bytes2Send = ((bufSize - MAX_BUF_SIZE * i) > MAX_BUF_SIZE)
                              ? MAX_BUF_SIZE
                              : (bufSize % MAX_BUF_SIZE);

    IoContext* context = new IoContext();
    context->iocpEevent = IocpEvent::Send;
    // context->buffer.buf = new char[bufSize];
    // context->buffer.len = bufSize;
    memcpy(context->buffer.buf, p, bytes2Send);
    context->buffer.len = bytes2Send;
    p += bytes2Send;
    DWORD dwOfBytesSent = 0;
    int re = WSASend(this->m_socket.getHandle(), &context->buffer, 1,
                     &dwOfBytesSent, 0, context, NULL);
    // cout << "send ret:" << re << endl;
    if (SOCKET_ERROR == re && WSAGetLastError() != WSA_IO_PENDING) {
      // cout << "send failed" << endl;
      // this->m_socket.shutdown();
      // this->m_socket.close();
      this->postCloseEvent();

      delete context;
    }
  }
}

bool TcpConnection::postCloseEvent() {
  if (this->m_isServer) return false;
  IoContext* context = new IoContext();
  context->iocpEevent = IocpEvent::Close;
  return this->m_iocp->postQueuedCompletionStatus(
      0, (ULONG_PTR) & this->m_iocpCtx, context);
}

TcpConnection& TcpConnection::onAccept(const OnAcceptFunc& func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onAcceptFunc = func;
  return *this;
}

TcpConnection& TcpConnection::onData(const OnDataFunc& func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onDataFunc = func;

  return *this;
}

TcpConnection& TcpConnection::onClose(const OnCloseFunc& func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onCloseFunc = func;
  return *this;
}

#else

TcpConnection::TcpConnection() : m_isServer(false), m_readBuffer(MAX_BUF_SIZE) {
  this->m_socket.setUseSelect(false);
  this->m_ep = &PollMgr::instance().getEPoll();
}

TcpConnection::TcpConnection(int sock)
    : m_isServer(false), m_socket(sock), m_readBuffer(MAX_BUF_SIZE) {
  this->m_socket.setUseSelect(false);
  this->m_ep = &PollMgr::instance().getEPoll();
  this->m_ep->addFd(this->m_socket.getHandle(),
                    Bind(&TcpConnection::onSocketUpdate, this));
}

TcpConnection::~TcpConnection() {
  this->m_ep->delFd(this->m_socket.getHandle());
  this->m_socket.close();
}

bool TcpConnection::isServer() const { return this->m_isServer; }

void TcpConnection::resetCallbackFunctions() {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onDataFunc = nullptr;
  this->m_onCloseFunc = nullptr;
  this->m_onAcceptFunc = nullptr;
  this->m_sendBuffers.clear();
}

bool TcpConnection::listen(uint16_t port, const std::string& ip,
                           IpVersion ipVer, int backlog) {
  // this->resetCallbackFunctions();

  try {
    if (!this->m_socket.createTcp(ipVer))
      THROW_EXCEPTION(Exception, "Create socket failed.", errno);
    if (!this->m_socket.bind(port, ip))
      THROW_EXCEPTION(
          Exception,
          SW("Bind to ")(port)(":")(ip)(" failed. ")(strerror(errno)), errno);
    this->m_socket.listen(backlog);
  } catch (std::exception& e) {
    this->m_socket.close();
    return false;
  }

  this->m_isServer = true;
  this->m_ep->addFd(this->m_socket.getHandle(),
                    Bind(&TcpConnection::onSocketUpdate, this));
  this->m_ep->addEvents(this->m_socket.getHandle(), POLLIN);
  return true;
}

bool TcpConnection::connect(uint16_t remotePort, const std::string& remoteHost,
                            uint16_t localPort, const std::string& localIp) {
  // this->resetCallbackFunctions();

  this->m_isServer = false;
  try {
    IpVersion ipVer = IpVersion::V4;
    SockAddr addr(remoteHost, remotePort);
    if (addr.isValid()) {
      ipVer = addr.getVersion();
    }

    if (!this->m_socket.createTcp(ipVer))
      THROW_EXCEPTION(Exception, "Create socket failed.", errno);

    if (localPort > 0) {
      if (!this->m_socket.bind(localPort, localIp))
        THROW_EXCEPTION(Exception,
                        SW("Bind to ")(localPort)(
                            ":")(localIp)(" failed. ")(strerror(errno)),
                        errno);
    }

    if (!this->m_socket.connect(remoteHost, remotePort, 5000)) {
      THROW_EXCEPTION(Exception,
                      SW("Connect to ")(remoteHost)(
                          ":")(remotePort)(" failed.")(strerror(errno)),
                      errno);
    }

  } catch (std::exception& e) {
    this->m_socket.close();
    return false;
  }

  this->m_ep->addFd(this->m_socket.getHandle(),
                    Bind(&TcpConnection::onSocketUpdate, this));
  this->m_ep->addEvents(this->m_socket.getHandle(), POLLIN);
  return true;
}

void TcpConnection::close() {
  this->m_socket.shutdown();

  OnCloseFunc func;
  {
    lock_guard<mutex> lk(this->m_mutex);
    func = this->m_onCloseFunc;
  }
  if (!func) {
    this->m_ep->delFd(this->m_socket.getHandle());
    this->m_socket.close();
  }
}

Socket& TcpConnection::getSocket() { return this->m_socket; }

void TcpConnection::send(const void* buf, uint32_t bufSize) {
  if (this->m_isServer) return;

  lock_guard<mutex> lk(this->m_sendMutex);
  this->m_sendBuffers.push_back(BufferPtr(new Buffer(buf, bufSize)));

  if (1 == this->m_sendBuffers.size())
    this->m_ep->addEvents(this->m_socket.getHandle(), POLLOUT);
}

TcpConnection& TcpConnection::onAccept(const OnAcceptFunc& func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onAcceptFunc = func;
  this->m_ep->addEvents(this->m_socket.getHandle(), POLLIN);
  return *this;
}

TcpConnection& TcpConnection::onData(const OnDataFunc& func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onDataFunc = func;
  this->m_ep->addEvents(this->m_socket.getHandle(), POLLIN);
  return *this;
}

TcpConnection& TcpConnection::onClose(const OnCloseFunc& func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onCloseFunc = func;
  this->m_ep->addEvents(this->m_socket.getHandle(), POLLHUP);
  return *this;
}

void TcpConnection::onSvrSocketUpdate(int fd, int events) {
  if (events & POLLIN) {
    SockAddr addr;
    auto sock = this->m_socket.accept(addr);
    if (sock.getHandle() <= 0) return;
    TcpConnectionPtr tcpSock =
        TcpConnectionPtr(new TcpConnection(sock.getHandle()));
    sock.dettach();

    OnAcceptFunc func;
    {
      lock_guard<mutex> lk(this->m_mutex);
      func = m_onAcceptFunc;
    }
    if (func) func(tcpSock);
  }

  if (events & POLLHUP) {
    this->m_ep->delFd(this->m_socket.getHandle());
    this->m_socket.close();

    OnCloseFunc func;
    {
      lock_guard<mutex> lk(this->m_mutex);
      func = m_onCloseFunc;
    }
    if (func) func(this->shared_from_this());
  }
}

void TcpConnection::onClientSocketUpdate(int fd, int events) {
  if (events & POLLHUP) {
    this->m_ep->delFd(this->m_socket.getHandle());

    OnCloseFunc func;
    {
      lock_guard<mutex> lk(this->m_mutex);
      func = m_onCloseFunc;
    }
    if (func) func(this->shared_from_this());
    // this->m_socket.close();
    return;
  }

  if (events & POLLIN) {
    int readBytes =
        this->m_socket.read(m_readBuffer.getData(), m_readBuffer.getSize());
    if (readBytes <= 0) {
      this->m_socket.shutdown();
    } else {
      OnDataFunc func;
      {
        lock_guard<mutex> lk(this->m_mutex);
        func = this->m_onDataFunc;
      }
      if (func)
        func((const uint8_t*)m_readBuffer.getData(), readBytes,
             this->shared_from_this());
    }
  }

  if (events & POLLOUT) {
    BufferPtr buffer;
    {
      lock_guard<mutex> lk(this->m_sendMutex);
      if (!this->m_sendBuffers.empty()) {
        buffer = this->m_sendBuffers.front();
        this->m_sendBuffers.pop_front();
      } else {
        this->m_ep->delEvents(this->m_socket.getHandle(), POLLOUT);
      }
    }

    if (buffer) {
      this->m_socket.send(buffer->getData(), buffer->getSize());
    }
  }
}

void TcpConnection::onSocketUpdate(int fd, int events) {
  (this->m_isServer ? this->onSvrSocketUpdate(fd, events)
                    : this->onClientSocketUpdate(fd, events));
}

TcpConnectionPtr TcpConnection::CreateInstance() {
  return TcpConnectionPtr(new TcpConnection);
}

#endif

NS_FF_END
