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
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/errno.h>
#include <fcntl.h>

#define sockopt_flat_t int
#endif

#include <sstream>
#include <cstring>
#include <iostream>
#include <ff/IP.h>

using namespace std;

namespace NS_FF {

SockAddr_t& SockAddr::getAddr() {
	return m_addr;
}

const SockAddr_t& SockAddr::getAddr() const {
	return m_addr;
}

IpVersion SockAddr::getVersion() const {
	return m_version;
}

void SockAddr::setVersion(IpVersion version) {
	this->m_version = version;
}

uint16_t SockAddr::getPort() const {
	if (this->m_version == IpVersion::V4) {
		return ntohs(this->m_addr.V4()->sin_port);
	}

	if (this->m_version == IpVersion::V6) {
		return ntohs(this->m_addr.V6()->sin6_port);
	}

	return 0;
}

void SockAddr::setPort(uint16_t port) {
	if (this->m_version == IpVersion::V4) {
		this->m_addr.V4()->sin_port = htons(port);
	}

	if (this->m_version == IpVersion::V6) {
		this->m_addr.V6()->sin6_port = htons(port);
	}
}

bool SockAddr::isValid() const {
	return (IpVersion::Unknown != this->m_version);
}

SockAddr::SockAddr() :
		m_version(IpVersion::Unknown) {
	memset(&this->m_addr, 0, sizeof(this->m_addr));
}

SockAddr::SockAddr(const std::string& host, uint16_t port) {
	memset(&this->m_addr, 0, sizeof(this->m_addr));

	auto sockAddr = Socket::Host2SockAddr(host);
	if (!sockAddr.isValid())
		return;

	*this = sockAddr;
	if (IpVersion::V4 == sockAddr.getVersion()) {
		this->m_addr.V4()->sin_port = htons(port);
		return;
	}

	if (IpVersion::V6 == sockAddr.getVersion()) {
		this->m_addr.V6()->sin6_port = htons(port);
		return;
	}
}

SockAddr::SockAddr(const SockAddr_t& addr, IpVersion version) :
		m_addr(addr), m_version(version) {
}

SockAddr Socket::Host2SockAddr(const std::string& host) {
	SockAddr sockAddr;
	if (host.empty())
		return sockAddr;

	struct addrinfo *answer, hints, *addr_info_p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (0 != getaddrinfo(host.c_str(), NULL, &hints, &answer))
		return sockAddr;

	for (addr_info_p = answer; addr_info_p != NULL;
			addr_info_p = addr_info_p->ai_next) {
		if (addr_info_p->ai_family == AF_INET) {
			auto sinp4 = (struct sockaddr_in *) addr_info_p->ai_addr;
			SockAddr_t addr_t;
			(*addr_t.V4()) = *sinp4;
			sockAddr = SockAddr(addr_t, IpVersion::V4);
		} else if (addr_info_p->ai_family == AF_INET6) {
			auto sinp6 = (struct sockaddr_in6 *) addr_info_p->ai_addr;
			SockAddr_t addr_t;
			(*addr_t.V6()) = *sinp6;
			sockAddr = SockAddr(addr_t, IpVersion::V6);
		}
	}
	freeaddrinfo(answer);
	return sockAddr;
}

std::string Socket::Host2IpStr(const std::string& host) {
	if (host.empty())
		return "";

	struct addrinfo *answer, hints, *addr_info_p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (0 != getaddrinfo(host.c_str(), NULL, &hints, &answer))
		return host;

	string ip;
	for (addr_info_p = answer; addr_info_p != NULL;
			addr_info_p = addr_info_p->ai_next) {
		if (addr_info_p->ai_family == AF_INET) {
			auto sinp4 = (struct sockaddr_in *) addr_info_p->ai_addr;
			ip = inet_ntoa(sinp4->sin_addr);
		} else if (addr_info_p->ai_family == AF_INET6) {
			auto sinp6 = (struct sockaddr_in6 *) addr_info_p->ai_addr;
			char ipv6_addr[64] = { 0 };

			sprintf(ipv6_addr,
					"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
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
	return Host2SockAddr(host).getAddr().V4()->sin_addr.s_addr;
}

in6_addr Socket::Host2IpV6(const std::string& host) {
	return Host2SockAddr(host).getAddr().V6()->sin6_addr;
}

Socket::Socket() :
		m_socketFd(0), m_useSelect(true), m_ipVer(IpVersion::Unknown){
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);
#endif
}

Socket::Socket(int m_socketFd) :
		m_useSelect(true) {
	this->m_socketFd = m_socketFd;
	this->m_ipVer = this->getIpVersion();
}

Socket::Socket(Socket&& sock) {
	this->close();
	this->m_socketFd = sock.m_socketFd;
	this->m_useSelect = sock.m_useSelect;
	this->m_ipVer = sock.m_ipVer;
	sock.m_socketFd = 0;
}

Socket::~Socket() {
	this->close();
}

Socket& Socket::attach(int m_socketFd) {
	this->close();
	this->m_socketFd = m_socketFd;
	return *this;
}

int Socket::dettach() {
	int oldSock = this->m_socketFd;
	this->m_socketFd = 0;
	return oldSock;
}

bool Socket::create(int af, int style, int protocol) {
	if (this->m_socketFd > 0) {
		this->close();
	}
	this->m_socketFd = socket(af, style, protocol);

	if (this->m_socketFd <= 0) {
		return false;
	}

#ifdef WIN32
	if (SOCK_DGRAM == style) {
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(this->m_socketFd, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior,
				NULL, 0, &dwBytesReturned, NULL, NULL);
	}
#endif

	return true;
}

Socket& Socket::setBlocking(bool nonBlocking) {
	if (this->m_socketFd > 0) {
		Socket::SetBlocking(this->m_socketFd, nonBlocking);
	}
	return *this;
}

bool Socket::isNonBlocking() const {
	return (this->m_socketFd > 0 && Socket::IsNonBlocking(this->m_socketFd));
}

int Socket::getHandle() {
	return this->m_socketFd;
}

Socket& Socket::shutdown(int type) {
	if (this->m_socketFd > 0) {
		::shutdown(this->m_socketFd, type);
	}
	return *this;
}

bool Socket::close() {
	if (this->m_socketFd <= 0)
		return true;

	int re = 0;
	this->shutdown();
#if defined(WIN32) || defined(__MINGW32__)
	re = ::closesocket(this->m_socketFd);
#else
	re = ::close(this->m_socketFd);
#endif
	this->m_socketFd = 0;

	return (0 == re);
}

bool Socket::createTcp(IpVersion ver) {
	if (IpVersion::Unknown == ver)
		return false;
	this->m_ipVer = ver;
	return this->create(IpVersion::V4 == ver ? PF_INET : PF_INET6, SOCK_STREAM);
}

bool Socket::createUdp(IpVersion ver) {
	if (IpVersion::Unknown == ver)
		return false;
	this->m_ipVer = ver;
	return this->create(IpVersion::V4 == ver ? PF_INET : PF_INET6, SOCK_DGRAM);
}

bool Socket::connect(const std::string& host, u16 port, int msTimeout) {
	if (this->m_socketFd <= 0) {
		return false;
	}

	bool re = false;
	SockAddr sockAddr = Socket::Host2SockAddr(host);
	if (!sockAddr.isValid())
		return false;

	sockAddr.setPort(port);
	const sockaddr* pAddr = (const sockaddr*) &sockAddr.getAddr();
	socklen_t addrSize =
			(IpVersion::V4 == sockAddr.getVersion()) ?
					sizeof(sockaddr_in) : sizeof(sockaddr_in6);
	bool nonBlock = this->isNonBlocking();
	this->setBlocking(true);

	int ret = ::connect(this->m_socketFd, pAddr, addrSize);
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
			getsockopt(this->m_socketFd, SOL_SOCKET, SO_ERROR, (char *) &error,
					&len);
			re = (0 == error);
		}
	}
	this->setBlocking(nonBlock);
	return re;
}

bool Socket::bind(u16 port, const std::string& ip) {
	if (this->m_socketFd <= 0)
		return false;

	const sockaddr* pAddr = nullptr;
	socklen_t addrSize = 0;

	sockaddr_in sockaddrV4;
	sockaddr_in6 sockaddrV6;
	// auto ipVer = this->getIpVersion();
	if (IpVersion::V4 == this->m_ipVer) {
		memset(&sockaddrV4, 0, sizeof(sockaddrV4));
		addrSize = sizeof(sockaddrV4);
		sockaddrV4.sin_family = PF_INET;
		sockaddrV4.sin_port = htons(port);
		if (!ip.empty()) {
#if defined(WIN32) || defined(__MINGW32__)
			sockaddrV4.sin_addr.s_addr = inet_addr(ip.c_str());
#else
			inet_aton(ip.c_str(), &sockaddrV4.sin_addr);
#endif
		} else {
			sockaddrV4.sin_addr.s_addr = INADDR_ANY;
		}

		pAddr = (const sockaddr*) &sockaddrV4;
	}

	if (IpVersion::V6 == this->m_ipVer) {
		int32_t flag = 1;
		if (this->setsockOpt(IPPROTO_IPV6, IPV6_V6ONLY, &flag, sizeof(flag))
				< 0)
			return false;

		memset(&sockaddrV6, 0, sizeof(sockaddrV6));
		addrSize = sizeof(sockaddrV6);
		if (!ip.empty()) {
			sockaddrV6 = *Socket::Host2SockAddr(ip).getAddr().V6();
		} else {
			sockaddrV6.sin6_addr = in6addr_any;
		}
		sockaddrV6.sin6_port = htons(port);

		pAddr = (const sockaddr*) &sockaddrV6;
	}

	if (nullptr == pAddr)
		return false;

	sockopt_flat_t flag = 1;
	setsockopt(this->m_socketFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	return (0 == ::bind(this->m_socketFd, pAddr, addrSize));
}

bool Socket::listen(int n) {
	return (0 == ::listen(this->m_socketFd, n));
}

Socket Socket::accept(sockaddr* addr, socklen_t* addrSize) {
	if (this->m_socketFd <= 0)
		return 0;

	return ::accept(this->m_socketFd, addr, addrSize);
}

Socket Socket::accept(SockAddr& addr) {
	addr.setVersion(this->m_ipVer);
	switch (this->m_ipVer) {
	case IpVersion::V4:
		return this->accept(*addr.getAddr().V4());
	case IpVersion::V6:
		return this->accept(*addr.getAddr().V6());
	default:
		break;
	}
	return Socket();
}

Socket Socket::accept(sockaddr_in& addr) {
	socklen_t size = sizeof(addr);
	return this->accept((sockaddr*) &addr, &size);
}

Socket Socket::accept(sockaddr_in6& addr) {
	socklen_t size = sizeof(addr);
	return this->accept((sockaddr*) &addr, &size);
}

int Socket::send(const void* buf, socklen_t bufLen) {
	if (this->m_socketFd <= 0)
		return -1;

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
	return ::send(this->m_socketFd, (const char*)buf, bufLen, 0);
}

int Socket::read(void* buf, socklen_t readBytes, int timeoutMs) {
	if (this->m_socketFd <= 0)
		return -1;
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
		size_t addrSize) {
	if (this->m_socketFd <= 0)
		return -1;

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

bool Socket::isConnected() {
	return (this->getRemotePort() > 0);
}

string Socket::getLocalAddress() const {
	if (this->m_socketFd <= 0)
		return "";
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getsockname(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return inet_ntoa(addr.sin_addr);
}

int Socket::getLocalPort() const {
	if (this->m_socketFd <= 0)
		return 0;
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getsockname(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return ntohs(addr.sin_port);
}

string Socket::getRemoteAddress() const {
	if (this->m_socketFd <= 0)
		return "";
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getpeername(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return inet_ntoa(addr.sin_addr);
}

int Socket::getRemotePort() const {
	if (this->m_socketFd <= 0)
		return 0;
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getpeername(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return ntohs(addr.sin_port);
}

SockType Socket::getSocketType() const {
	if (this->m_socketFd <= 0) {
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

IpVersion Socket::getIpVersion() const {
	if (this->m_socketFd <= 0)
		return IpVersion::Unknown;

	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	if (-1 == ::getsockname(this->m_socketFd, (sockaddr*) &addr, &addrLen))
		return IpVersion::Unknown;
	return (PF_INET6 == addr.sin_family ? IpVersion::V6 : IpVersion::V4);
}

int Socket::getSockOpt(int level, int optName, void* optVal,
socklen_t* optLen) {
	return ::getsockopt(this->m_socketFd, level, optName, (char*)optVal, optLen);
}

int Socket::setsockOpt(int level, int optName, const void* optVal,
socklen_t optLen) {
	return ::setsockopt(this->m_socketFd, level, optName, (const char*)optVal, optLen);
}

int Socket::sendTo(const char* buf, socklen_t bufLen, const string& host,
		int port) {
	if (host.empty() || port <= 0) {
		return -1;
	}

	SockAddr sockAddr(host, port);
	if (!sockAddr.isValid())
		return -1;

	const sockaddr* pAddr = (const sockaddr*) &sockAddr.getAddr();
	size_t addrSize =
			IpVersion::V6 == sockAddr.getVersion() ?
					sizeof(sockaddr_in6) : sizeof(sockaddr_in);
	return this->sendTo(buf, bufLen, pAddr, addrSize);
}

int Socket::recvFrom(char* buf, socklen_t readBytes, sockaddr_in& addr,
		int timeoutMs) {
	if (this->m_socketFd <= 0)
		return -1;

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
	return ::recvfrom(this->m_socketFd, buf, readBytes, 0, (sockaddr*) &addr,
			&addrSize);
}

int Socket::recvFrom(char* buf, socklen_t readBytes, string& ip, int& port,
		int timeoutMs) {
	ip = "";
	port = 0;

	sockaddr_in addr = { 0 };
	int re = this->recvFrom(buf, readBytes, addr, timeoutMs);
	if (re > 0) {
		ip = inet_ntoa(addr.sin_addr);
		port = ntohs(addr.sin_port);
	}

	return re;
}

void Socket::enableUdpBroadcast(bool enable)
{
	const int on = enable ? 1 : 0;
	this->setsockOpt(SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
}

bool Socket::SetBlocking(int m_socketFd, bool isNonBlocking) {
#if defined(WIN32) || defined(__MINGW32__)
	unsigned long isFIONBIO = 1;
	return (0 == ioctlsocket(m_socketFd, FIONBIO, &isFIONBIO));
#else
	int opts = fcntl(m_socketFd, F_GETFL);
	if (opts < 0) {
		return false;
	}
	if (isNonBlocking) {
		opts |= O_NONBLOCK;
	} else {
		opts &= ~O_NONBLOCK;
	}

	return (fcntl(m_socketFd, F_SETFL, opts) >= 0);
#endif
}

bool Socket::IsNonBlocking(int m_socketFd) {
#if defined(WIN32) || defined(__MINGW32__)
	unsigned long isFIONBIO = 1;
	if(-1 == ioctlsocket(m_socketFd, FIONREAD, &isFIONBIO)) return false;
	return (1 == isFIONBIO);
#else
	int opts = fcntl(m_socketFd, F_GETFL);
	if (opts < 0) {
		return false;
	}
	return (O_NONBLOCK == (opts * O_NONBLOCK));
#endif
}

bool Socket::isUseSelect() const {
	return m_useSelect;
}

Socket& Socket::setUseSelect(bool m_useSelect) {
	this->m_useSelect = m_useSelect;
	return *this;
}

bool Socket::setNoDelay(bool nodelay) {
	int32_t flag = nodelay ? 1 : 0;
	int32_t result = setsockopt(this->m_socketFd, IPPROTO_TCP, TCP_NODELAY,
			(char *) &flag, sizeof(int));
	return (result >= 0);
}

bool Socket::setKeepAlive(bool keepAlive, uint32_t idle, uint32_t interval,
		uint32_t count) {
	if (keepAlive) {
		int32_t val = 1;
		if (this->setsockOpt(SOL_SOCKET, SO_KEEPALIVE,
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
		if (this->setsockOpt(SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val)) != 0)
			return false;
#else
		(void)interval;
#endif

#if defined(SOL_TCP) && defined(TCP_KEEPCNT)
		val = count;
		if (this->setsockOpt(SOL_TCP, TCP_KEEPCNT, &val, sizeof(val)) != 0)
			return false;
#else
		(void)count;
#endif
	} else {
		int32_t val = 0;
		if (this->setsockOpt(SOL_SOCKET, SO_KEEPALIVE,
				reinterpret_cast<const char*>(&val), sizeof(val)) != 0)
			return false;
	}

	return true;
}

} /* namespace NS_FF */
