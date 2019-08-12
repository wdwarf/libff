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
#else
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#endif

#include <sstream>
#include <cstring>
#include <iostream>

using namespace std;

namespace NS_FF {

namespace {

unsigned int Host2Ip(const std::string& host){
	hostent* he = gethostbyname(host.c_str());
	if (he && (he->h_length > 0)) {
		unsigned char* pAddr = (unsigned char*) he->h_addr_list[0];
		if (pAddr) {
			return *((unsigned int*) pAddr);
		}
	}

	return inet_addr(host.c_str());
}

}

Socket::Socket() :
		m_socketFd(0), m_useSelect(true) {
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);
#endif
}

Socket::Socket(int m_socketFd) :
		m_useSelect(true) {
	this->m_socketFd = m_socketFd;
}

Socket::~Socket() {
}

bool Socket::attach(int m_socketFd) {
	this->close();
	this->m_socketFd = m_socketFd;
	return true;
}

int Socket::dettach() {
	int oldSock = this->m_socketFd;
	this->m_socketFd = 0;
	return oldSock;
}

int Socket::create(int af, int style, int protocol) {
	if (this->m_socketFd > 0) {
		this->close();
	}
	this->m_socketFd = socket(af, style, protocol);

	if (this->m_socketFd <= 0) {
		THROW_EXCEPTION(SocketException,
				string("can't create socket.") + strerror(errno), errno);
	}

#ifdef WIN32
	if (SOCK_DGRAM == style) {
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(iSock, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior,
				NULL, 0, &dwBytesReturned, NULL, NULL);
	}
#endif

	return this->m_socketFd;
}

void Socket::setBlocking(bool nonBlocking) {
	if (this->m_socketFd > 0) {
		Socket::SetBlocking(this->m_socketFd, nonBlocking);
	}
}

bool Socket::isNonBlocking() const {
	return (this->m_socketFd > 0 && Socket::IsNonBlocking(this->m_socketFd));
}

int Socket::getHandle() {
	return this->m_socketFd;
}

int Socket::shutdown(int type) {
	if (this->m_socketFd > 0) {
		return ::shutdown(this->m_socketFd, type);
	}
	return 0;
}

int Socket::close() {
	int re = 0;
	if (this->m_socketFd > 0) {
		this->shutdown();
#if defined(WIN32) || defined(__MINGW32__)
		re = ::closesocket(this->m_socketFd);
#else
		re = ::close(this->m_socketFd);
#endif
		this->m_socketFd = 0;
	}
	return re;
}

int Socket::createTcp() {
	return this->create(AF_INET, SOCK_STREAM);
}

int Socket::createUdp() {
	return this->create(AF_INET, SOCK_DGRAM);
}

int Socket::connect(const std::string& host, int port, int msTimeout) {
	if (this->m_socketFd <= 0) {
		return -1;
	}

	string ip;
	int re = -1;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = Host2Ip(host);

	bool nonBlock = this->isNonBlocking();
	this->setBlocking(true);

	re = ::connect(this->m_socketFd, (sockaddr*) &addr, sizeof(sockaddr));
	if (-1 == re) {
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
			re = select(this->m_socketFd + 1, NULL, &set, NULL, ptm);
		} while (re < 0 && EINTR == errno);

		if (1 == re) {
			socklen_t len = sizeof(int);
			getsockopt(this->m_socketFd, SOL_SOCKET, SO_ERROR, (char *) &error,
					&len);
			re = (0 == errno) ? 0 : -1;
		} else {
			re = -1;
		}
	}
	this->setBlocking(nonBlock);
	return re;
}

void Socket::bind(int port, const std::string& ip) {
	int re = -1;
	if (this->m_socketFd > 0) {
		if (port <= 0) {
			THROW_EXCEPTION(SocketException,
					string("invalid binding port: ") + (string)Variant(port) + "." + strerror(errno),
					errno);
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		if (!ip.empty()) {
#if defined(WIN32) || defined(__MINGW32__)
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
#else
			inet_aton(ip.c_str(), &addr.sin_addr);
#endif
		} else {
			addr.sin_addr.s_addr = INADDR_ANY;
		}

		int flag = 1;
		setsockopt(this->m_socketFd, SOL_SOCKET, SO_REUSEADDR, &flag,
				sizeof(flag));

		re = ::bind(this->m_socketFd, (sockaddr*) &addr, sizeof(sockaddr));
		if (re < 0) {
			THROW_EXCEPTION(SocketException,
					"bind [" + ip+ ":" + (string)Variant(port) + "] failed." + strerror(errno),
					errno);
		}
	}
}

void Socket::listen(int n) {
	if (this->m_socketFd > 0) {
		if (0 != ::listen(this->m_socketFd, n)) {
			THROW_EXCEPTION(SocketException,
					string("listen failed.") + strerror(errno), errno);
		}
	}
}

Socket Socket::accept(sockaddr_in& addr) {
	int s_fd = 0;
	if (this->m_socketFd > 0) {
		socklen_t addrLen = sizeof(addr);
		s_fd = ::accept(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	}
	return s_fd;
}

int Socket::send(const char* buf, socklen_t bufLen) {
	int re = -1;
	if (this->m_socketFd > 0) {
		if (this->m_useSelect) {
			fd_set fs_send;
			timeval tv;
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			FD_ZERO(&fs_send);
			FD_SET(this->m_socketFd, &fs_send);
			re = ::select(this->m_socketFd + 1, 0, &fs_send, 0, &tv);
			if (re <= 0) {
				return re;
			}
		}
		re = ::send(this->m_socketFd, buf, bufLen, 0);
	}
	return re;
}

int Socket::read(char* buf, socklen_t readBytes, int timeoutMs) {
	int re = -1;
	if (this->m_socketFd > 0) {
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
		re = ::recv(this->m_socketFd, buf, readBytes, 0);
#else
		re = ::read(this->m_socketFd, buf, readBytes);
#endif
	}
	return re;
}

int Socket::sendTo(const char* buf, socklen_t bufLen, const sockaddr_in& addr) {
	int re = -1;
	if (this->m_socketFd > 0) {
		if (this->m_useSelect) {
			fd_set fs_send;
			timeval tv;
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			FD_ZERO(&fs_send);
			FD_SET(this->m_socketFd, &fs_send);
			re = ::select(this->m_socketFd + 1, 0, &fs_send, 0, &tv);
			if (re <= 0) {
				return re;
			}
		}
		re = ::sendto(this->m_socketFd, buf, bufLen, 0, (sockaddr*) &addr,
				sizeof(sockaddr));
	}
	return re;
}

bool Socket::isConnected() {
	return (this->getRemotePort() > 0);
}

string Socket::getLocalAddress() {
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getsockname(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return inet_ntoa(addr.sin_addr);
}

int Socket::getLocalPort() {
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getsockname(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return ntohs(addr.sin_port);
}

string Socket::getRemoteAddress() {
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getpeername(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return inet_ntoa(addr.sin_addr);
}

int Socket::getRemotePort() {
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	::getpeername(this->m_socketFd, (sockaddr*) &addr, &addrLen);
	return ntohs(addr.sin_port);
}

SockType Socket::getSocketType() {
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

int Socket::getSockOpt(int level, int optName, void* optVal,
socklen_t* optLen) {
	return ::getsockopt(this->m_socketFd, level, optName, optVal, optLen);
}

int Socket::setsockOpt(int level, int optName, const void* optVal,
socklen_t optLen) {
	return ::setsockopt(this->m_socketFd, level, optName, optVal, optLen);
}

int Socket::sendTo(const char* buf, socklen_t bufLen, const string& host,
		int port) {
	if (host.empty() || port <= 0) {
		return -1;
	}

	string realIp;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = Host2Ip(host);

	return this->sendTo(buf, bufLen, addr);
}

int Socket::recvFrom(char* buf, socklen_t readBytes, sockaddr_in& addr,
		int timeoutMs) {
	int re = -1;
	if (this->m_socketFd > 0) {
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

		memset(&addr, 0, sizeof(addr));
		socklen_t addrSize = sizeof(addr);
		re = ::recvfrom(this->m_socketFd, buf, readBytes, 0, (sockaddr*) &addr,
				&addrSize);
	}
	return re;
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

void Socket::setUseSelect(bool m_useSelect) {
	this->m_useSelect = m_useSelect;
}

} /* namespace NS_FF */
