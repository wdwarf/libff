/*
 * Socket.h
 *
 *  Created on: Aug 9, 2019
 *      Author: ducky
 */

#ifndef FF_SOCKET_H_
#define FF_SOCKET_H_

#include <ff/Object.h>
#include <ff/Exception.h>
#include <string>
#include <memory>

#ifdef WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#define SHUT_RDWR 2

#else

#include <sys/socket.h>
#include <netinet/in.h>

#endif

#ifndef socklen_t

#ifdef WIN32
#define socklen_t int
#else
#define socklen_t unsigned int
#endif

#endif

namespace NS_FF {

typedef int SocketFd;

#if 0
enum class SocketType {
	Stream = SOCK_STREAM,
	Dgram = SOCK_DGRAM,
	Raw = SOCK_RAW,
	Rdm = SOCK_RDM,
	Seqpacket = SOCK_SEQPACKET,
	Dccp = SOCK_DCCP,
	Packet = SOCK_PACKET,
	Cloexec = SOCK_CLOEXEC,
	Nonblock = SOCK_NONBLOCK
};
#endif

typedef __socket_type SockType;

enum class IpVersion : char{
	Unknown,
	V4,
	V6
};

union SockAddr_t{
	sockaddr_in sockaddrV4;
	sockaddr_in6 sockaddrV6;
};

class SockAddr{
public:
	SockAddr();
	SockAddr(const std::string& host, uint16_t port);
	SockAddr(const SockAddr_t& addr, IpVersion version);

	const SockAddr_t& getAddr() const;
	IpVersion getVersion() const;
	uint16_t getPort() const;
	void setPort(uint16_t port);

	bool isValid() const;

private:
	SockAddr_t m_addr;
	IpVersion m_version;
};

class FFDLL Socket {
public:
	Socket();
	Socket(int sockFd);
	Socket(Socket&& sock);
	~Socket();

	bool create(int af, int style, int protocol = 0);
	bool createTcp(IpVersion ver = IpVersion::V4);
	bool createUdp(IpVersion ver = IpVersion::V4);
	int getHandle();
	bool close();
	Socket& shutdown(int type = SHUT_RDWR);
	Socket& attach(int sockFd);
	int dettach();
	Socket& setBlocking(bool nonBlocking);
	bool isNonBlocking() const;
	bool isUseSelect() const;
	Socket& setUseSelect(bool useSelect);

	std::string getLocalAddress();
	int getLocalPort();
	std::string getRemoteAddress();
	int getRemotePort();
	SockType getSocketType();
	IpVersion getIpVersion() const;

	int getSockOpt(int level, int optName, void* optVal, socklen_t* optLen);
	int setsockOpt(int level, int optName, const void* optVal, socklen_t optLen);
	bool setNoDelay(bool nodelay);
	bool setKeepAlive(bool keepAlive, uint32_t idle = 10,
			uint32_t interval = 10, uint32_t count = 9);

	bool connect(const std::string& host, u16 port, int msTimeout = 3000);
	bool isConnected();
	bool bind(u16 port, const std::string& ip = "");
	bool listen(int n = 10);
	Socket accept(sockaddr_in& addr);
	Socket accept(sockaddr_in6& addr);
	Socket accept(sockaddr* addr, socklen_t* addrSize);

	int send(const void* buf, socklen_t bufLen);
	int read(void* buf, socklen_t readBytes, int timeoutMs = -1);

	int sendTo(const char* buf, socklen_t bufLen, const sockaddr* addr, size_t addrSize);
	int sendTo(const char* buf, socklen_t bufLen, const std::string& host,
			int port);
	int recvFrom(char* buf, socklen_t readBytes, sockaddr_in& addr,
			int timeoutMs = -1);
	int recvFrom(char* buf, socklen_t readBytes, std::string& ip, int& port,
			int timeoutMs = -1);

	static bool SetBlocking(int sockFd, bool isNonBlocking = true);
	static bool IsNonBlocking(int sockFd);

	static std::string Host2IpStr(const std::string& host);
	static in_addr_t Host2Ip(const std::string& host);
	static in6_addr Host2IpV6(const std::string& host);
	static SockAddr Host2SockAddr(const std::string& host);
private:
	SocketFd m_socketFd;
	bool m_useSelect;
};

typedef std::shared_ptr<Socket> SocketPtr;

} /* namespace NS_FF */

#endif /* FF_SOCKET_H_ */
