/*
 * TcpClient.cpp
 *
 *  Created on: Aug 12, 2019
 *      Author: root
 */

#include <ff/TcpClient.h>
#include <ff/StringWrapper.h>
#include <cstring>
#include <errno.h>
#include <iostream>

using namespace std;

namespace NS_FF {

ClientEventContext::ClientEventContext() : m_event(NetEvent::UNKNOWN) {

}

ClientEventContext::ClientEventContext(NetEvent event, BufferPtr buffer) :
		m_event(event), m_buffer(buffer) {

}

const BufferPtr& ClientEventContext::getBuffer() const {
	return m_buffer;
}

void ClientEventContext::setBuffer(const BufferPtr &buffer) {
	m_buffer = buffer;
}

NetEvent ClientEventContext::getEvent() const {
	return m_event;
}

void ClientEventContext::setEvent(NetEvent event) {
	m_event = event;
}

TcpClient::TcpClient() :
		m_remotePort(0), m_localPort(0), m_stoped(true) {
	this->m_eventThread = thread(&TcpClient::eventThreadFunc, this);
}

TcpClient::~TcpClient() {
	this->stop();
	this->addEvent(ClientEventContext(NetEvent::EXIT));
	if (this->m_eventThread.joinable())
	{
		this->m_eventThread.join();
	}
}

void TcpClient::onStart() {
}

void TcpClient::onStartFailed(const std::string &errInfo) {
}

void TcpClient::onStop() {
	cout << "on stop" << endl;
}

void TcpClient::onConnected() {
}

void TcpClient::onDisconnected() {
}

void TcpClient::onRecv(const BufferPtr &buffer) {
}

void TcpClient::onSend(const BufferPtr &buffer) {
}

bool TcpClient::start() {
	if (!this->m_stoped)
		return true;

	this->m_stoped = false;

	this->addEvent(ClientEventContext(NetEvent::START));
	return true;
}

bool TcpClient::stop() {
	if(this->m_stoped)
		return true;

	this->addEvent(ClientEventContext(NetEvent::STOP));

	if (this->m_sendThread.joinable())
		this->m_sendThread.join();
	if (this->m_recvThread.joinable())
		this->m_recvThread.join();

	return true;
}

void TcpClient::send(const void *buf, u32 bufSize) {
	unique_lock<mutex> lk(this->m_mutexSend);
	this->m_sendBufferList.push_back(
			make_shared<Buffer>((const char*) buf, bufSize));
	this->m_condSend.notify_one();
}

void TcpClient::addEvent(const ClientEventContext &evt) {
	unique_lock<mutex> lk(this->m_mutexEvent);
	this->m_events.push_back(evt);
	this->m_condEvent.notify_one();
}

void TcpClient::eventThreadFunc() {
	cout << __func__ << endl;
	while (true) {
		ClientEventContext event;
		{
			unique_lock<mutex> lk(this->m_mutexEvent);
			if (this->m_events.empty()) {
				this->m_condEvent.wait(lk);
			}

			if (this->m_events.empty())
				continue;

			event = this->m_events.front();
			this->m_events.pop_front();
		}

		if(NetEvent::EXIT == event.getEvent()) {
			break;
		}

		if(this->m_stoped)
			continue;

		switch (event.getEvent()) {
		case NetEvent::RECV: {
			this->onRecv(event.getBuffer());
			break;
		}
		case NetEvent::SEND: {
			this->onSend(event.getBuffer());
			break;
		}
		case NetEvent::START: {
			this->doStart();
			break;
		}
		case NetEvent::STOP: {
			{
				unique_lock<mutex> lk(this->m_mutexSend);
				this->m_stoped = true;
				this->m_sendBufferList.clear();
				this->m_condSend.notify_one();
				this->m_socket.shutdown();
			}

			this->onStop();

			this->m_socket.close();

			break;
		}
		case NetEvent::CONNECTED: {
			this->onConnected();
			break;
		}
		case NetEvent::DISCONNECTED: {
			this->onDisconnected();
			break;
		}
		default: {
			break;
		}
		}
	}
}

void TcpClient::doStart() {
	cout << __func__ << endl;
	try {
		this->doConnect();
	} catch (std::exception &e) {
		this->onStartFailed(e.what());
		return;
	}

	this->m_recvThread = thread(&TcpClient::recvThreadFunc, this);
	this->m_sendThread = thread(&TcpClient::sendThreadFunc, this);

	this->onStart();
}

const std::string& TcpClient::getLocalIp() const {
	return m_localIp;
}

void TcpClient::setLocalIp(const std::string &localIp) {
	m_localIp = localIp;
}

unsigned int TcpClient::getLocalPort() const {
	return m_localPort;
}

void TcpClient::setLocalPort(unsigned int localPort) {
	m_localPort = localPort;
}

const std::string& TcpClient::getRemoteAddr() const {
	return m_remoteAddr;
}

void TcpClient::setRemoteAddr(const std::string &remoteAddr) {
	m_remoteAddr = remoteAddr;
}

unsigned int TcpClient::getRemotePort() const {
	return m_remotePort;
}

void TcpClient::setRemotePort(unsigned int remotePort) {
	m_remotePort = remotePort;
}

void TcpClient::doConnect() {
	if (this->m_socket.createTcp() <= 0)
		THROW_EXCEPTION(Exception, "Create socket failed.", errno);

	if (this->m_localPort > 0) {
		if (!this->m_socket.bind(this->m_localPort, this->m_localIp))
			THROW_EXCEPTION(Exception,
					SW("Bind to ")(this->m_remoteAddr)(":") (this->m_remotePort)(" failed. ")(strerror(errno)),
					errno);
	}

	if (!this->m_socket.connect(this->m_remoteAddr, this->m_remotePort, 5000)) {
		THROW_EXCEPTION(Exception,
				SW("Connect to ")(this->m_remoteAddr)(":")(this->m_remotePort)(
						" failed."), errno);
	}

	this->addEvent(ClientEventContext(NetEvent::CONNECTED));
}

void TcpClient::recvThreadFunc() {
	Buffer buf(1024 * 4);
	while (!this->m_stoped) {
		int readBytes = this->m_socket.read(buf.getData(), buf.getSize());
		if (readBytes <= 0)
			break;

		this->addEvent(
				ClientEventContext(NetEvent::RECV,
						make_shared<Buffer>(buf.getData(), readBytes)));
	}

	this->addEvent(ClientEventContext(NetEvent::DISCONNECTED));
	cout << "recv therad end" << endl;
}

void TcpClient::sendThreadFunc() {
	while (!this->m_stoped) {
		BufferPtr buffer;
		{
			unique_lock<mutex> lk(this->m_mutexSend);
			if (this->m_sendBufferList.empty()) {
				this->m_condSend.wait(lk);
			}

			if(this->m_stoped)
				break;

			if (this->m_sendBufferList.empty())
				continue;

			buffer = this->m_sendBufferList.front();
			this->m_sendBufferList.pop_front();
		}

		if (!buffer)
			continue;

		if (this->m_socket.send(buffer->getData(), buffer->getSize()) > 0) {
			this->addEvent(ClientEventContext(NetEvent::SEND, buffer));
		}
	}
	cout << "send therad end" << endl;
}

} /* namespace NS_FF */
