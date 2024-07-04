/*
 * Socket.cpp
 *
 *  Created on: Aug 9, 2019
 *      Author: ducky
 */

#include <ff/Socket.h>
#include <ff/Variant.h>

#ifdef WIN32
#include <Mstcpip.h>
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
#define sockopt_flat_t char

#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define sockopt_flat_t int
#endif

#include <ff/IP.h>

#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

NS_FF_BEG

namespace {

class __Init__ {
 public:
  __Init__() {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  }
};
static __Init__ __init__;

}  // namespace

SockAddr::SockAddr() { memset(&this->m_addr, 0, sizeof(this->m_addr)); }

SockAddr::SockAddr(const std::string& host, uint16_t port) {
  memset(&this->m_addr, 0, sizeof(this->m_addr));
  *this = Socket::Host2SockAddr(host);
  this->setPort(port);
}

sockaddr_in* SockAddr::v4() { return reinterpret_cast<sockaddr_in*>(&m_addr); }
const sockaddr_in* const SockAddr::v4() const {
  return reinterpret_cast<const sockaddr_in* const>(&m_addr);
}

SockAddr::operator sockaddr*() { return reinterpret_cast<sockaddr*>(&m_addr); }
SockAddr::operator const sockaddr* const() const {
  return reinterpret_cast<const sockaddr* const>(&m_addr);
}

SockAddr::operator sockaddr_in*() { return this->v4(); }
SockAddr::operator const sockaddr_in* const() const { return this->v4(); }
SockAddr::operator sockaddr_in() const { return *this->v4(); }

sockaddr_in6* SockAddr::v6() {
  return reinterpret_cast<sockaddr_in6*>(&m_addr);
}
const sockaddr_in6* const SockAddr::v6() const {
  return reinterpret_cast<const sockaddr_in6* const>(&m_addr);
}

SockAddr::operator sockaddr_in6*() { return this->v6(); }
SockAddr::operator const sockaddr_in6* const() const { return this->v6(); }
SockAddr::operator sockaddr_in6() const { return *this->v6(); }

uint16_t SockAddr::getFamily() const { return this->m_addr.ss_family; }
void SockAddr::setFamily(uint16_t family) { this->m_addr.ss_family = family; }

uint16_t SockAddr::getPort() const {
  if (this->getFamily() == AF_INET) {
    return ntohs(this->v4()->sin_port);
  }

  if (this->getFamily() == AF_INET6) {
    return ntohs(this->v6()->sin6_port);
  }

  return 0;
}

void SockAddr::setPort(uint16_t port) {
  if (this->getFamily() == AF_INET) {
    this->v4()->sin_port = htons(port);
  }

  if (this->getFamily() == AF_INET6) {
    this->v6()->sin6_port = htons(port);
  }
}

std::string SockAddr::getIp() const {
  char addr[INET6_ADDRSTRLEN] = {0};
  if (AF_INET == this->getFamily()) {
    inet_ntop(AF_INET, (void*)&this->v4()->sin_addr, addr, INET6_ADDRSTRLEN);
  }

  if (AF_INET6 == this->getFamily()) {
    inet_ntop(AF_INET6, (void*)&this->v6()->sin6_addr, addr, INET6_ADDRSTRLEN);
  }

  return addr;
}

bool SockAddr::isValid() const { return (AF_UNSPEC != this->getFamily()); }

SockAddr Socket::Host2SockAddr(const std::string& host) {
  SockAddr sockAddr;
  if (host.empty()) return sockAddr;

  struct addrinfo *answer, hints, *addr_info_p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (0 != getaddrinfo(host.c_str(), NULL, &hints, &answer)) return sockAddr;

  for (addr_info_p = answer; addr_info_p != NULL;
       addr_info_p = addr_info_p->ai_next) {
    if (addr_info_p->ai_family == AF_INET) {
      auto sinp4 = (struct sockaddr_in*)addr_info_p->ai_addr;

      (*sockAddr.v4()) = *sinp4;
    } else if (addr_info_p->ai_family == AF_INET6) {
      auto sinp6 = (struct sockaddr_in6*)addr_info_p->ai_addr;
      (*sockAddr.v6()) = *sinp6;
    }
  }

  freeaddrinfo(answer);
  return sockAddr;
}

std::string Socket::Host2IpStr(const std::string& host) {
  if (host.empty()) return "";

  struct addrinfo *answer, hints, *addr_info_p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (0 != getaddrinfo(host.c_str(), NULL, &hints, &answer)) return host;

  string ip;
  for (addr_info_p = answer; addr_info_p != NULL;
       addr_info_p = addr_info_p->ai_next) {
    if (addr_info_p->ai_family == AF_INET) {
      auto sinp4 = (struct sockaddr_in*)addr_info_p->ai_addr;
      ip = inet_ntoa(sinp4->sin_addr);
    } else if (addr_info_p->ai_family == AF_INET6) {
      auto sinp6 = (struct sockaddr_in6*)addr_info_p->ai_addr;
      char ipv6_addr[64] = {0};

      sprintf(ipv6_addr,
              "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"
              "02x%02x",
              sinp6->sin6_addr.s6_addr[0], sinp6->sin6_addr.s6_addr[1],
              sinp6->sin6_addr.s6_addr[2], sinp6->sin6_addr.s6_addr[3],
              sinp6->sin6_addr.s6_addr[4], sinp6->sin6_addr.s6_addr[5],
              sinp6->sin6_addr.s6_addr[6], sinp6->sin6_addr.s6_addr[7],
              sinp6->sin6_addr.s6_addr[8], sinp6->sin6_addr.s6_addr[9],
              sinp6->sin6_addr.s6_addr[10], sinp6->sin6_addr.s6_addr[11],
              sinp6->sin6_addr.s6_addr[12], sinp6->sin6_addr.s6_addr[13],
              sinp6->sin6_addr.s6_addr[14], sinp6->sin6_addr.s6_addr[15]);
      ip = ipv6_addr;

      if (sinp6->sin6_scope_id > 0) {
        ip += "%" + std::to_string(sinp6->sin6_scope_id);
      }
    }
  }
  freeaddrinfo(answer);
  return ip;
}

in_addr_t Socket::Host2Ip(const std::string& host) {
  return Host2SockAddr(host).v4()->sin_addr.s_addr;
}

in6_addr Socket::Host2IpV6(const std::string& host) {
  return Host2SockAddr(host).v6()->sin6_addr;
}

Socket::Socket()
    : m_socketFd(INVALID_SOCKET),
      m_useSelect(true),
      m_blockingType(SockBlockingType::Blocking) {}

Socket::Socket(int socketFd) : m_useSelect(true) {
  this->m_socketFd = socketFd;
  this->m_blockingType = this->isNonBlocking() ? SockBlockingType::NonBlocking
                                               : SockBlockingType::Blocking;
}

Socket::Socket(Socket&& sock) {
  this->close();
  this->m_socketFd = sock.m_socketFd;
  this->m_useSelect = sock.m_useSelect;
  this->m_blockingType = sock.m_blockingType;
  sock.m_socketFd = INVALID_SOCKET;
}

Socket::~Socket() { this->close(); }

Socket& Socket::attach(int m_socketFd) {
  this->close();
  this->m_socketFd = m_socketFd;
  this->m_blockingType = this->isNonBlocking() ? SockBlockingType::NonBlocking
                                               : SockBlockingType::Blocking;
  return *this;
}

SocketFd Socket::dettach() {
  SocketFd oldSock = this->m_socketFd;
  this->m_socketFd = INVALID_SOCKET;
  return oldSock;
}

bool Socket::create(int af, int style, int protocol) {
  if (INVALID_SOCKET != this->m_socketFd) {
    this->close();
  }
  this->m_socketFd = socket(af, style, protocol);

  if (INVALID_SOCKET == this->m_socketFd) {
    return false;
  }

#ifdef WIN32
  if (SOCK_DGRAM == style) {
    BOOL bNewBehavior = FALSE;
    DWORD dwBytesReturned = 0;
    WSAIoctl(this->m_socketFd, SIO_UDP_CONNRESET, &bNewBehavior,
             sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);
  }
#endif

  Socket::SetBlocking(this->m_socketFd, this->m_blockingType);
  return true;
}

Socket& Socket::setBlocking(SockBlockingType blockingType) {
  if (INVALID_SOCKET != this->m_socketFd) {
    Socket::SetBlocking(this->m_socketFd, blockingType);
  }
  this->m_blockingType = blockingType;
  return *this;
}

bool Socket::isNonBlocking() const {
  return (INVALID_SOCKET != this->m_socketFd &&
          Socket::IsNonBlocking(this->m_socketFd));
}

SocketFd Socket::getHandle() const { return this->m_socketFd; }

Socket& Socket::shutdown(int type) {
  if (INVALID_SOCKET != this->m_socketFd) {
    ::shutdown(this->m_socketFd, type);
  }
  return *this;
}

bool Socket::close() {
  if (INVALID_SOCKET == this->m_socketFd) return true;

  int re = 0;
  this->shutdown();
#if defined(WIN32) || defined(__MINGW32__)
  re = ::closesocket(this->m_socketFd);
#else
  re = ::close(this->m_socketFd);
#endif
  this->m_socketFd = INVALID_SOCKET;

  return (0 == re);
}

bool Socket::createTcp(int family) {
  if (AF_UNSPEC == family) return false;
  return this->create(AF_INET == family ? PF_INET : PF_INET6, SOCK_STREAM);
}

bool Socket::createUdp(int family) {
  if (AF_UNSPEC == family) return false;
  return this->create(AF_INET == family ? PF_INET : PF_INET6, SOCK_DGRAM);
}

bool Socket::connect(const sockaddr* addr, int addrLen, int msTimeout) {
  if (INVALID_SOCKET == this->m_socketFd) {
    return false;
  }

  bool re = false;

  bool nonBlock = this->isNonBlocking();
  Socket::SetBlocking(this->m_socketFd, SockBlockingType::NonBlocking);

  int ret = ::connect(this->m_socketFd, addr, addrLen);
  if (-1 == ret) {
    timeval tm;
    timeval* ptm = NULL;
    fd_set set;
    int error = -1;
    if (msTimeout >= 0) {
      ptm = &tm;
      tm.tv_sec = msTimeout / 1000;
      tm.tv_usec = (msTimeout % 1000) * 1000;
    }
    FD_ZERO(&set);
    FD_SET(this->m_socketFd, &set);
    do {
      ret = select(this->m_socketFd + 1, NULL, &set, NULL, ptm);
    } while (ret < 0 && EINTR == errno);

    if (1 == ret) {
      socklen_t len = sizeof(int);
      getsockopt(this->m_socketFd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
      re = (0 == error);
    }
  }
  Socket::SetBlocking(this->m_socketFd, this->m_blockingType);

  return re;
}

bool Socket::connect(const std::string& host, uint16_t port, int msTimeout) {
  if (INVALID_SOCKET == this->m_socketFd) {
    return false;
  }

  bool re = false;
  SockAddr sockAddr = Socket::Host2SockAddr(host);
  if (!sockAddr.isValid()) return false;

  sockAddr.setPort(port);
  const sockaddr* pAddr = sockAddr;
  socklen_t addrSize = (AF_INET == sockAddr.getFamily()) ? sizeof(sockaddr_in)
                                                         : sizeof(sockaddr_in6);
  bool nonBlock = this->isNonBlocking();
  Socket::SetBlocking(this->m_socketFd, SockBlockingType::NonBlocking);

  return this->connect(pAddr, addrSize, msTimeout);
}

bool Socket::bind(const sockaddr* addr, int addrLen) {
  return (0 == ::bind(this->getHandle(), addr, addrLen));
}

bool Socket::bind(uint16_t port, const std::string& ip) {
  if (INVALID_SOCKET == this->m_socketFd) return false;

  SockAddr addr;
  socklen_t addrSize = sizeof(addr);
  auto family = this->getLocalSockAddr().getFamily();
  addr.setFamily(family);

  if (AF_INET == family) {
    if (!ip.empty()) {
      inet_pton(AF_INET, ip.c_str(), &addr.v4()->sin_addr);
    } else {
      addr.v4()->sin_addr.s_addr = INADDR_ANY;
    }
  }

  if (AF_INET6 == family) {
    int32_t flag = 1;
    if (this->setSockOpt(IPPROTO_IPV6, IPV6_V6ONLY, &flag, sizeof(flag)) < 0)
      return false;

    if (!ip.empty()) {
      addr = Socket::Host2SockAddr(ip);
    } else {
      addr.v6()->sin6_addr = in6addr_any;
    }
  }

  addr.setPort(port);

  sockopt_flat_t flag = 1;
  setsockopt(this->m_socketFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  return (0 == ::bind(this->m_socketFd, addr, addrSize));
}

bool Socket::joinMulticastGroup(const std::string& ip) {
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(ip.c_str());
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  return (setsockopt(this->m_socketFd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                     (char*)&mreq, sizeof(mreq)) >= 0);
}

bool Socket::listen(int n) { return (0 == ::listen(this->m_socketFd, n)); }

Socket Socket::accept(sockaddr* addr, socklen_t* addrSize) {
  if (INVALID_SOCKET == this->m_socketFd) return 0;

  return ::accept(this->m_socketFd, addr, addrSize);
}

Socket Socket::accept(SockAddr& addr) {
  socklen_t size = sizeof(addr);
  return this->accept(addr, &size);
}

Socket Socket::accept(sockaddr_in& addr) {
  socklen_t size = sizeof(addr);
  return this->accept((sockaddr*)&addr, &size);
}

Socket Socket::accept(sockaddr_in6& addr) {
  socklen_t size = sizeof(addr);
  return this->accept((sockaddr*)&addr, &size);
}

int Socket::getReadableBytes() {
#ifdef _WIN32
  u_long n = 0;
  if (ioctlsocket(this->m_socketFd, FIONREAD, &n) < 0) return -1;
  return (int)n;
#else
  int n = -1;
  if (ioctl(this->m_socketFd, FIONREAD, &n) < 0) return -1;
  return n;
#endif
}

int Socket::send(const void* buf, socklen_t bufLen, int timeoutMs) {
  if (INVALID_SOCKET == this->m_socketFd) return -1;

  if (this->m_useSelect) {
    fd_set fs_send;
    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    FD_ZERO(&fs_send);
    FD_SET(this->m_socketFd, &fs_send);
    int re = ::select(this->m_socketFd + 1, 0, &fs_send, 0, &tv);
    if (re <= 0) {
      return re;
    }
  }
  return ::send(this->m_socketFd, (const char*)buf, bufLen, 0);
}

int Socket::read(void* buf, socklen_t readBytes, int timeoutMs) {
  if (INVALID_SOCKET == this->m_socketFd) return -1;
  int re = -1;

  if (this->m_useSelect) {
    fd_set fs_read;
    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    FD_ZERO(&fs_read);
    FD_SET(this->m_socketFd, &fs_read);
    re = ::select(this->m_socketFd + 1, &fs_read, 0, 0,
                  (-1 == timeoutMs ? 0 : &tv));
    if (re <= 0) {
      return re;
    }
  }

#if defined(WIN32) || defined(__MINGW32__)
  re = ::recv(this->m_socketFd, (char*)buf, readBytes, 0);
#else
  re = ::read(this->m_socketFd, buf, readBytes);
#endif

  return re;
}

int Socket::sendTo(const char* buf, socklen_t bufLen, const sockaddr* addr,
                   int addrSize) {
  if (INVALID_SOCKET == this->m_socketFd) return -1;

  if (this->m_useSelect) {
    fd_set fs_send;
    timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    FD_ZERO(&fs_send);
    FD_SET(this->m_socketFd, &fs_send);
    int re = ::select(this->m_socketFd + 1, 0, &fs_send, 0, &tv);
    if (re <= 0) {
      return re;
    }
  }
  return ::sendto(this->m_socketFd, buf, bufLen, 0, addr, addrSize);
}

bool Socket::isConnected() const { return (this->getRemotePort() > 0); }

SockAddr Socket::getLocalSockAddr() const {
  SockAddr addr;
  if (INVALID_SOCKET == this->m_socketFd) return addr;
  socklen_t addrLen = sizeof(addr);
  int re = ::getsockname(this->m_socketFd, addr, &addrLen);
  if (re < 0) {
#if defined(WIN32) || defined(__MINGW32__)
    WSAPROTOCOL_INFO pto;
    int size = sizeof(pto);
    ::getsockopt(this->m_socketFd, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&pto,
                 &size);
    addr.setFamily(pto.iAddressFamily);
#else
    int family = 0;
    socklen_t size = sizeof(int);
    ::getsockopt(this->m_socketFd, SOL_SOCKET, SO_PROTOCOL, &family, &size);
    addr.setFamily(family);
#endif
  }

  return addr;
}

string Socket::getLocalAddress() const {
  if (INVALID_SOCKET == this->m_socketFd) return "";
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t addrLen = sizeof(addr);
  if (::getsockname(this->m_socketFd, (sockaddr*)&addr, &addrLen) < 0)
    return "";
  return inet_ntoa(addr.sin_addr);
}

int Socket::getLocalPort() const {
  if (INVALID_SOCKET == this->m_socketFd) return -1;
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t addrLen = sizeof(addr);
  if (::getsockname(this->m_socketFd, (sockaddr*)&addr, &addrLen) < 0)
    return -1;
  return ntohs(addr.sin_port);
}

string Socket::getRemoteAddress() const {
  if (INVALID_SOCKET == this->m_socketFd) return "";
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t addrLen = sizeof(addr);
  if (::getpeername(this->m_socketFd, (sockaddr*)&addr, &addrLen) < 0)
    return "";
  return inet_ntoa(addr.sin_addr);
}

int Socket::getRemotePort() const {
  if (INVALID_SOCKET == this->m_socketFd) return -1;
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t addrLen = sizeof(addr);
  if (::getpeername(this->m_socketFd, (sockaddr*)&addr, &addrLen) < 0)
    return -1;
  return ntohs(addr.sin_port);
}

SockType Socket::getSocketType() const {
  if (INVALID_SOCKET == this->m_socketFd) {
    return SockType(-1);
  }
#if defined(WIN32) || defined(__MINGW32__)
  char type = -1;
#else
  int type = -1;
#endif
  socklen_t len = sizeof(type);
  ::getsockopt(this->m_socketFd, SOL_SOCKET, SO_TYPE, &type, &len);
  return SockType(type);
}

int Socket::getSockOpt(int level, int optName, void* optVal,
                       socklen_t* optLen) {
  return ::getsockopt(this->m_socketFd, level, optName, (char*)optVal, optLen);
}

int Socket::setSockOpt(int level, int optName, const void* optVal,
                       socklen_t optLen) {
  return ::setsockopt(this->m_socketFd, level, optName, (const char*)optVal,
                      optLen);
}

int Socket::sendTo(const char* buf, socklen_t bufLen, const string& host,
                   int port) {
  if (host.empty() || port <= 0) {
    return -1;
  }

  SockAddr sockAddr(host, port);
  if (!sockAddr.isValid()) return -1;

  return this->sendTo(buf, bufLen, sockAddr, sizeof(sockAddr));
}

int Socket::recvFrom(char* buf, socklen_t readBytes, sockaddr_in& addr,
                     int timeoutMs) {
  if (INVALID_SOCKET == this->m_socketFd) return -1;

  if (this->m_useSelect) {
    fd_set fs_read;
    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    FD_ZERO(&fs_read);
    FD_SET(this->m_socketFd, &fs_read);
    int re = ::select(this->m_socketFd + 1, &fs_read, 0, 0,
                      (-1 == timeoutMs ? 0 : &tv));
    if (re <= 0) {
      return re;
    }
  }

  memset(&addr, 0, sizeof(addr));
  socklen_t addrSize = sizeof(addr);
  return ::recvfrom(this->m_socketFd, buf, readBytes, 0, (sockaddr*)&addr,
                    &addrSize);
}

int Socket::recvFrom(char* buf, socklen_t readBytes, string& ip, int& port,
                     int timeoutMs) {
  ip = "";
  port = 0;

  sockaddr_in addr = {0};
  int re = this->recvFrom(buf, readBytes, addr, timeoutMs);
  if (re > 0) {
    ip = inet_ntoa(addr.sin_addr);
    port = ntohs(addr.sin_port);
  }

  return re;
}

void Socket::enableUdpBroadcast(bool enable) {
  const int on = enable ? 1 : 0;
  this->setSockOpt(SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
}

bool Socket::enableMulticast(const std::string& multiAddr, 
  const std::string interfaceAddr){
    struct ip_mreq mc;
    memset(&mc, 0, sizeof(mc));
    inet_pton(AF_INET, multiAddr.c_str(), &mc.imr_multiaddr.s_addr);
    inet_pton(AF_INET, interfaceAddr.c_str(), &mc.imr_interface.s_addr);
    return (0 == this->setSockOpt(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mc, sizeof(mc)));
}

bool Socket::SetBlocking(SocketFd m_socketFd, SockBlockingType blockingType) {
#if defined(WIN32) || defined(__MINGW32__)
  unsigned long isFIONBIO = blockingType == SockBlockingType::Blocking ? 0 : 1;
  return (0 == ioctlsocket(m_socketFd, FIONBIO, &isFIONBIO));
#else
  int opts = fcntl(m_socketFd, F_GETFL);
  if (opts < 0) {
    return false;
  }
  if (blockingType == SockBlockingType::NonBlocking) {
    opts |= O_NONBLOCK;
  } else {
    opts &= ~O_NONBLOCK;
  }

  return (fcntl(m_socketFd, F_SETFL, opts) >= 0);
#endif
}

bool Socket::IsNonBlocking(SocketFd m_socketFd) {
#if defined(WIN32) || defined(__MINGW32__)
  unsigned long isFIONBIO = 0;
  if (-1 == ioctlsocket(m_socketFd, FIONREAD, &isFIONBIO)) return false;
  return (1 == isFIONBIO);
#else
  int opts = fcntl(m_socketFd, F_GETFL);
  if (opts < 0) {
    return false;
  }
  return (O_NONBLOCK == (opts * O_NONBLOCK));
#endif
}

bool Socket::isUseSelect() const { return m_useSelect; }

Socket& Socket::setUseSelect(bool m_useSelect) {
  this->m_useSelect = m_useSelect;
  return *this;
}

bool Socket::setNoDelay(bool nodelay) {
  int32_t flag = nodelay ? 1 : 0;
  int32_t result = setsockopt(this->m_socketFd, IPPROTO_TCP, TCP_NODELAY,
                              (char*)&flag, sizeof(int));
  return (result >= 0);
}

bool Socket::setKeepAlive(bool keepAlive, uint32_t idle, uint32_t interval,
                          uint32_t count) {
  if (keepAlive) {
    int32_t val = 1;
    if (this->setSockOpt(SOL_SOCKET, SO_KEEPALIVE,
                         reinterpret_cast<const char*>(&val), sizeof(val)) != 0)
      return false;

#if defined(SOL_TCP) && defined(TCP_KEEPIDLE)
    val = idle;
    if (setsockopt(this->m_socketFd, SOL_TCP, TCP_KEEPIDLE, &val,
                   sizeof(val)) != 0)
      return false;
#else
    (void)idle;
#endif

#if defined(SOL_TCP) && defined(TCP_KEEPINTVL)
    val = interval;
    if (this->setSockOpt(SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val)) != 0)
      return false;
#else
    (void)interval;
#endif

#if defined(SOL_TCP) && defined(TCP_KEEPCNT)
    val = count;
    if (this->setSockOpt(SOL_TCP, TCP_KEEPCNT, &val, sizeof(val)) != 0)
      return false;
#else
    (void)count;
#endif
  } else {
    int32_t val = 0;
    if (this->setSockOpt(SOL_SOCKET, SO_KEEPALIVE,
                         reinterpret_cast<const char*>(&val), sizeof(val)) != 0)
      return false;
  }

  return true;
}

bool Socket::SocketPair(SocketFd fdPair[2], int family, int type,
                        int protocol) {
#ifndef _WIN32
  return (0 == socketpair(family, type, protocol, fdPair));
#endif
  Socket sockListen;
  Socket sockConn;
  if (!sockListen.create(AF_INET, type, 0) ||
      !sockConn.create(AF_INET, type, 0))
    return false;

  sockaddr_in addrListen;
  memset(&addrListen, 0, sizeof(addrListen));
  addrListen.sin_family = AF_INET;
  addrListen.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addrListen.sin_port = 0;

  if (!sockListen.bind((sockaddr*)&addrListen, sizeof(addrListen)) ||
      !sockListen.listen(1))
    return false;

  sockaddr_in addrConn;
  memset(&addrConn, 0, sizeof(addrConn));
  socklen_t size = sizeof(addrConn);
  if (-1 == getsockname(sockListen.getHandle(), (sockaddr*)&addrConn, &size))
    return false;

  if (!sockConn.connect((sockaddr*)&addrConn, size)) return false;

  auto sockAccept = sockListen.accept((sockaddr*)&addrListen, &size);
  if (INVALID_SOCKET == sockAccept.getHandle()) return false;

  if (-1 == getsockname(sockConn.getHandle(), (struct sockaddr*)&addrConn,
                        &size) ||
      size != sizeof(addrConn) ||
      addrListen.sin_family != addrConn.sin_family ||
      addrListen.sin_addr.s_addr != addrConn.sin_addr.s_addr ||
      addrListen.sin_port != addrConn.sin_port)
    return false;

  fdPair[0] = sockConn.dettach();
  fdPair[1] = sockAccept.dettach();

  return true;
}

NS_FF_END
