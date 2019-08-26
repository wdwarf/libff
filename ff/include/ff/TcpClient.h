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
#include <atomic>
#include <list>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace NS_FF {

enum class NetEvent {
	UNKNOWN, START, EXIT, CONNECTED, DISCONNECTED, RECV, SEND
};

class ClientEventContext {
public:
	ClientEventContext();
	ClientEventContext(NetEvent event, BufferPtr buffer = nullptr);

	const BufferPtr& getBuffer() const;
	void setBuffer(const BufferPtr& buffer);
	NetEvent getEvent() const;
	void setEvent(NetEvent event);

private:
	NetEvent m_event;
	BufferPtr m_buffer;
};

class ITcpClientEventListener{
public:
	ITcpClientEventListener() = default;
	virtual ~ITcpClientEventListener() = default;

	virtual void onStart() = 0;
	virtual void onStartFailed(const std::string& errInfo) = 0;
	virtual void onStop() = 0;
	virtual void onConnected() = 0;
	virtual void onDisconnected() = 0;
	virtual void onRecv(const BufferPtr& buffer) = 0;
	virtual void onSend(const BufferPtr& buffer) = 0;
};

class TcpClient: public ff::Object, public ff::Noncopyable {
public:
	TcpClient();
	virtual ~TcpClient();

	bool start();
	bool stop();
	void send(const void* buf, u32 bufSize);

	virtual void onStartFailed(const std::string& errInfo);
	virtual void onConnected();
	virtual void onDisconnected();
	virtual void onRecv(const BufferPtr& buffer);
	virtual void onSend(const BufferPtr& buffer);

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

	std::atomic_bool m_stoped;
	Socket m_socket;
	std::thread m_recvThread;
	std::thread m_sendThread;
	std::thread m_eventThread;

	void addEvent(const ClientEventContext& evt);

	void eventThreadFunc();
	void recvThreadFunc();
	void sendThreadFunc();
	std::list<ClientEventContext> m_events;
	std::mutex m_mutexEvent;
	std::condition_variable m_condEvent;

	std::list<BufferPtr> m_sendBufferList;
	std::atomic_bool m_sendThreadStop;
	std::mutex m_mutexSend;
	std::condition_variable m_condSend;

	void doStart();
	void doConnect() _throws(Exception);
};

typedef std::shared_ptr<TcpClient> TcpClientPtr;

} /* namespace NS_FF */

#endif /* FF_TCPCLIENT_H_ */
