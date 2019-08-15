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

ClientEventContext::ClientEventContext(){

}

ClientEventContext::ClientEventContext(NetEvent event, BufferPtr buffer) :
		m_event(event), m_buffer(buffer) {

}

const BufferPtr& ClientEventContext::getBuffer() const {
	return m_buffer;
}

void ClientEventContext::setBuffer(const BufferPtr& buffer) {
	m_buffer = buffer;
}

NetEvent ClientEventContext::getEvent() const {
	return m_event;
}

void ClientEventContext::setEvent(NetEvent event) {
	m_event = event;
}

TcpClient::TcpClient() :
		m_remotePort(0), m_localPort(0), m_eventStoped(true), m_ioStoped(true) {
}

TcpClient::~TcpClient() {
	this->stop();
	if(this->m_eventThread.joinable())
		this->m_eventThread.join();
}

bool TcpClient::start() {
	if (!this->m_eventStoped)
		return true;

	this->m_eventStoped = false;
	this->m_ioStoped = false;

	if(this->m_eventThread.joinable())
		this->m_eventThread.join();

	this->m_eventThread = thread(&TcpClient::eventThreadFunc, this);
	this->addEvent(ClientEventContext(NetEvent::START));
	return true;
}

bool TcpClient::stop() {
	this->addEvent(ClientEventContext(NetEvent::STOP));
	return true;
}

void TcpClient::send(const void* buf, u32 bufSize){
	unique_lock<mutex> lk(this->m_mutexSend);
	this->m_sendBufferList.push_back(
			make_shared<Buffer>((const char*) buf, bufSize));
	this->m_condSend.notify_one();
}

void TcpClient::addEvent(const ClientEventContext& evt) {
	unique_lock<mutex> lk(this->m_mutexEvent);
	this->m_events.push_back(evt);
	this->m_condEvent.notify_one();
}

void TcpClient::eventThreadFunc() {
	cout << __func__ << endl;
	while (!this->m_eventStoped) {
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
			this->m_eventStoped = true;
			{
				unique_lock<mutex> lk(this->m_mutexSend);
				this->m_ioStoped = true;
				this->m_sendBufferList.clear();
				this->m_condSend.notify_one();
				this->m_socket.shutdown();
			}

			if (this->m_sendThread.joinable())
				this->m_sendThread.join();
			if (this->m_recvThread.joinable())
				this->m_recvThread.join();

			this->m_events.clear();
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
		default:{
			break;
		}
		}
	}

	this->onStop();
}

void TcpClient::doStart() {
	cout << __func__ << endl;
	try {
		this->doConnect();
	} catch (std::exception& e) {
		this->onStartFailed(e.what());
		return;
	}

	this->m_recvThread = thread(&TcpClient::recvThreadFunc, this);
	this->m_eventThread = thread(&TcpClient::sendThreadFunc, this);

	this->onStart();
}

const std::string& TcpClient::getLocalIp() const {
	return m_localIp;
}

void TcpClient::setLocalIp(const std::string& localIp) {
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

void TcpClient::setRemoteAddr(const std::string& remoteAddr) {
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
		if(!this->m_socket.bind(this->m_localPort, this->m_localIp))
			THROW_EXCEPTION(Exception, SW("Bind to ")(this->m_remoteAddr)(":")
							(this->m_remotePort)(" failed. ")(strerror(errno)), errno);
	}

	if (!this->m_socket.connect(this->m_remoteAddr, this->m_remotePort, 5000)) {
		THROW_EXCEPTION(Exception, SW("Connect to ")(this->m_remoteAddr)(":")
				(this->m_remotePort)(" failed."), errno);
	}

	this->addEvent(ClientEventContext(NetEvent::CONNECTED));
}

void TcpClient::recvThreadFunc(){
	Buffer buf(1024 * 4);
	while(!this->m_ioStoped){
		int readBytes = this->m_socket.read(buf.getData(), buf.getSize());
		if(readBytes <= 0)
			break;

		this->addEvent(ClientEventContext(NetEvent::RECV,
				make_shared<Buffer>(buf.getData(), readBytes)));
	}

	this->addEvent(ClientEventContext(NetEvent::DISCONNECTED));
}

void TcpClient::sendThreadFunc(){
	while(!this->m_ioStoped){
		BufferPtr buffer;
		{
			unique_lock<mutex> lk(this->m_mutexSend);
			if (this->m_sendBufferList.empty()) {
				this->m_condSend.wait(lk);
			}

			if (this->m_events.empty())
				continue;

			buffer = this->m_sendBufferList.front();
			this->m_sendBufferList.pop_front();
		}

		if(!buffer)
			continue;

		if(this->m_socket.send(buffer->getData(), buffer->getSize()) > 0){
			this->addEvent(ClientEventContext(NetEvent::SEND, buffer));
		}
	}
}

} /* namespace NS_FF */
