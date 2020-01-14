/*
 * TcpClient.h
 *
 *  Created on: Aug 12, 2019
 *      Author: root
 */

#ifndef FF_TCPCLIENT_H_
#define FF_TCPCLIENT_H_

#include <ff/ff_config.h>
#include <ff/Noncopyable.h>
#include <ff/Exception.h>
#include <ff/Object.h>
#include <ff/Socket.h>
#include <ff/Thread.h>
#include <ff/Buffer.h>
#include <ff/EPoll.h>
#include <atomic>
#include <list>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace NS_FF {

class FFDLL TcpClient: public ff::Object, public ff::Noncopyable {
public:
	TcpClient();
	TcpClient(uint32_t recvBufSize);
	virtual ~TcpClient();

	void start();
	void stop();
	void send(const void* buf, u32 bufSize);

	virtual void onConnected();
	virtual void onDisconnected();
	virtual void onRecv(const uint8_t* buf, int bufLen);
	virtual void onSend(const uint8_t* buf, int bufLen);

	const std::string& getLocalIp() const;
	void setLocalIp(const std::string& localIp);
	unsigned int getLocalPort() const;
	void setLocalPort(unsigned int localPort);
	const std::string& getRemoteAddr() const;
	void setRemoteAddr(const std::string& remoteAddr);
	unsigned int getRemotePort() const;
	void setRemotePort(unsigned int remotePort);

private:
	std::string m_remoteAddr;
	unsigned int m_remotePort;
	std::string m_localIp;
	unsigned int m_localPort;
	Socket m_socket;
	Buffer m_readBuffer;
	bool m_closed;
	std::list<BufferPtr> m_sendBuffers;
	std::mutex m_sendMutex;

	void onSocketUpdate(int fd, int events);
};

typedef std::shared_ptr<TcpClient> TcpClientPtr;

} /* namespace NS_FF */

#endif /* FF_TCPCLIENT_H_ */
