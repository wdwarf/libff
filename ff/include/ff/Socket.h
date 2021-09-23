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
#define in_addr_t	ULONG

#else

#include <sys/socket.h>
#include <netinet/in.h>

#endif

#ifndef socklen_t

#ifdef _WIN32
#define socklen_t int
#else
#define socklen_t unsigned int
#endif

#endif

NS_FF_BEG

#ifdef _WIN32
	typedef SOCKET SocketFd;
#else
	typedef int SocketFd;
#endif

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

typedef int SockType;

enum class IpVersion : uint8_t {
	Unknown, V4, V6
};

enum class SockBlockingType: uint8_t{
	Blocking,
	NonBlocking
};

struct LIBFF_API SockAddr_t {
	sockaddr_storage sockaddr;

	sockaddr_in* V4() const {
		return (sockaddr_in*) &sockaddr;
	}

	sockaddr_in6* V6() const {
		return (sockaddr_in6*) &sockaddr;
	}
};

class LIBFF_API SockAddr {
public:
	SockAddr();
	SockAddr(const std::string& host, uint16_t port);
	SockAddr(const SockAddr_t& addr, IpVersion version);

	SockAddr_t& getAddr();
	const SockAddr_t& getAddr() const;
	IpVersion getVersion() const;
	void setVersion(IpVersion version);
	uint16_t getPort() const;
	void setPort(uint16_t port);

	bool isValid() const;

private:
	SockAddr_t m_addr;
	IpVersion m_version;
};

class LIBFF_API Socket {
public:
	Socket();
	Socket(int sockFd);
	Socket(Socket&& sock);
	~Socket();

	bool create(int af, int style, int protocol = 0);
	bool createTcp(IpVersion ver = IpVersion::V4);
	bool createUdp(IpVersion ver = IpVersion::V4);
	SocketFd getHandle() const;
	bool close();
	Socket& shutdown(int type = SHUT_RDWR);
	Socket& attach(int sockFd);
	SocketFd dettach();
	Socket& setBlocking(SockBlockingType blockingType);
	bool isNonBlocking() const;
	bool isUseSelect() const;
	Socket& setUseSelect(bool useSelect);

	std::string getLocalAddress() const;
	int getLocalPort() const;
	std::string getRemoteAddress() const;
	int getRemotePort() const;
	SockType getSocketType() const;
	IpVersion getIpVersion() const;

	int getSockOpt(int level, int optName, void* optVal, socklen_t* optLen);
	int setsockOpt(int level, int optName, const void* optVal,
			socklen_t optLen);
	bool setNoDelay(bool nodelay);
	bool setKeepAlive(bool keepAlive, uint32_t idle = 10,
			uint32_t interval = 10, uint32_t count = 9);

	bool connect(const std::string& host, uint16_t port, int msTimeout = 3000);
	bool isConnected() const;
	bool bind(uint16_t port, const std::string& ip = "");
	bool joinMulticastGroup(const std::string& ip);
	bool listen(int n = 10);
	Socket accept(SockAddr& addr);
	Socket accept(sockaddr_in& addr);
	Socket accept(sockaddr_in6& addr);
	Socket accept(sockaddr* addr, socklen_t* addrSize);

	int send(const void* buf, socklen_t bufLen, int timeoutMs = 10 * 1000);
	int read(void* buf, socklen_t readBytes, int timeoutMs = -1);

	int sendTo(const char* buf, socklen_t bufLen, const sockaddr* addr,
			size_t addrSize);
	int sendTo(const char* buf, socklen_t bufLen, const std::string& host,
			int port);
	int recvFrom(char* buf, socklen_t readBytes, sockaddr_in& addr,
			int timeoutMs = -1);
	int recvFrom(char* buf, socklen_t readBytes, std::string& ip, int& port,
			int timeoutMs = -1);
	void enableUdpBroadcast(bool enable = true);

	static bool SetBlocking(SocketFd sockFd, SockBlockingType blockingType);
	static bool IsNonBlocking(SocketFd sockFd);

	static std::string Host2IpStr(const std::string& host);
	static in_addr_t Host2Ip(const std::string& host);
	static in6_addr Host2IpV6(const std::string& host);
	static SockAddr Host2SockAddr(const std::string& host);
private:
	SocketFd m_socketFd;
	bool m_useSelect;
	IpVersion m_ipVer;
	SockBlockingType m_blockingType;
};

typedef std::shared_ptr<Socket> SocketPtr;

NS_FF_END

#endif /* FF_SOCKET_H_ */
