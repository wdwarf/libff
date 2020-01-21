/*
 * TcpServer.h
 *
 *  Created on: Jan 17, 2020
 *      Author: liyawu
 */

#ifndef FF_TCPCONNECTION_H_
#define FF_TCPCONNECTION_H_

#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <list>
#include <ff/ff_config.h>
#include <ff/Socket.h>
#include <ff/Buffer.h>
#include <ff/BlockingList.h>
#include <ff/EPoll.h>

namespace NS_FF {

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const uint8_t*, uint32_t, const TcpConnectionPtr&)> OnDataFunc;
typedef std::function<void(const TcpConnectionPtr&)> OnAcceptFunc;
typedef std::function<void(const TcpConnectionPtr&)> OnCloseFunc;

class TcpConnection: public std::enable_shared_from_this<TcpConnection> {
public:
	~TcpConnection();

	static TcpConnectionPtr CreateInstance();

	bool listen(uint16_t port, const std::string& ip = "", IpVersion ipVer =
			IpVersion::V4);
	bool isServer() const;
	bool connect(uint16_t remotePort, const std::string& remoteHost,
			uint16_t localPort = 0, const std::string& localIp = "");

	void send(const void* buf, u32 bufSize);
	TcpConnection& onData(const OnDataFunc& func);
	TcpConnection& onClose(const OnCloseFunc& func);
	TcpConnection& onAccept(const OnAcceptFunc& func);
	void close();

	const Socket& getSocket() const;
private:
	TcpConnection();
	TcpConnection(Socket&& socket);
	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(const TcpConnection&) = delete;

	EPoll* m_ep;
	bool m_isServer;
	Socket m_socket;
	Buffer m_readBuffer;
	std::list<BufferPtr> m_sendBuffers;
	OnDataFunc m_onDataFunc;
	OnCloseFunc m_onCloseFunc;
	OnAcceptFunc m_onAcceptFunc;
	std::mutex m_mutex;
	std::mutex m_sendMutex;

	void resetCallbackFunctions();
	void onSocketUpdate(int fd, int events);
	void onSvrSocketUpdate(int fd, int events);
	void onClientSocketUpdate(int fd, int events);
};

} /* namespace NS_FF */

#endif /* FF_TCPCONNECTION_H_ */
