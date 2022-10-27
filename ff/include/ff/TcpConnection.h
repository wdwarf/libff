/*
 * TcpConnection.h
 *
 *  Created on: Jan 17, 2020
 *      Author: liyawu
 */

#ifndef FF_TCPCONNECTION_H_
#define FF_TCPCONNECTION_H_

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <list>
#include <ff/ff_config.h>
#include <ff/Socket.h>
#include <ff/Buffer.h>
#include <ff/BlockingList.h>
#ifdef _WIN32
#include <ff/windows/IOCP.h>
#else
#include <ff/EPoll.h>
#endif

NS_FF_BEG

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const uint8_t*, uint32_t, const TcpConnectionPtr&)> OnDataFunc;
typedef std::function<void(const TcpConnectionPtr&)> OnAcceptFunc;
typedef std::function<void(const TcpConnectionPtr&)> OnConnectedFunc;
typedef std::function<void(const TcpConnectionPtr&)> OnCloseFunc;

class LIBFF_API TcpConnection: public std::enable_shared_from_this<TcpConnection> {
public:
	~TcpConnection();

#ifdef _WIN32
	static TcpConnectionPtr CreateInstance(IOCPPtr iocp = IOCPPtr(GIocp::getInstance(), [](void*){}));
#else
	static TcpConnectionPtr CreateInstance();
#endif

	bool listen(uint16_t port, const std::string& ip = "", uint16_t family = AF_INET, int backlog = 1000);
	bool isServer() const;
	bool connect(uint16_t remotePort, const std::string& remoteHost,
			uint16_t localPort = 0, const std::string& localIp = "");

	void send(const void* buf, uint32_t bufSize);
	TcpConnection& onData(const OnDataFunc& func);
	TcpConnection& onClose(const OnCloseFunc& func);
	TcpConnection& onAccept(const OnAcceptFunc& func);
	void close();

	Socket& getSocket();
private:
	
#ifdef _WIN32
	IocpContext m_iocpCtx;
	TcpConnectionPtr m_pThis;
	void active();
	TcpConnection(IOCPPtr iocp = IOCPPtr(GIocp::getInstance(), [](void*){}));
	TcpConnection(Socket&& socket, IOCPPtr iocp = IOCPPtr(GIocp::getInstance(), [](void*){}));
#else
	TcpConnection();
	TcpConnection(int sock);
#endif

	
	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(const TcpConnection&) = delete;

#ifdef _WIN32
	std::thread m_acceptThread;
	void workThreadFunc(DWORD numberOfBytesTransferred,
                       ULONG_PTR completionKey, LPOVERLAPPED lpOverlapped);
	bool postCloseEvent();
	IOCPPtr m_iocp;
#else
	EPoll* m_ep;
	
	void onSocketUpdate(int fd, int events);
	void onSvrSocketUpdate(int fd, int events);
	void onClientSocketUpdate(int fd, int events);
#endif
	bool m_isServer;
	Socket m_socket;
	Buffer m_readBuffer;
	std::list<BufferPtr> m_sendBuffers;
	std::list<BufferPtr> m_currSendBuffers;
	uint32_t m_currSendSize;
	int m_currSendBytes;
	OnDataFunc m_onDataFunc;
	OnCloseFunc m_onCloseFunc;
	OnAcceptFunc m_onAcceptFunc;
	std::mutex m_mutex;
	std::mutex m_sendMutex;

	void resetCallbackFunctions();

};

NS_FF_END

#endif /* FF_TCPCONNECTION_H_ */
