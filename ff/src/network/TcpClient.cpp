/*
 * TcpClient.cpp
 *
 *  Created on: Aug 12, 2019
 *      Author: root
 */

#include <ff/TcpClient.h>
#include <ff/StringWrapper.h>
#include <ff/Bind.h>
#include <cstring>
#include <errno.h>
#include <iostream>

using namespace std;

namespace NS_FF {

TcpClient::TcpClient() :
		m_remotePort(0), m_localPort(0), m_readBuffer(1024), m_closed(true) {
}

TcpClient::TcpClient(uint32_t recvBufSize) :
		m_remotePort(0), m_localPort(0), m_readBuffer(recvBufSize), m_closed(true) {
}

TcpClient::~TcpClient() {
	this->stop();
}

void TcpClient::onConnected() {
}

void TcpClient::onDisconnected() {
}

void TcpClient::onRecv(const uint8_t* buf, int bufLen) {
}

void TcpClient::onSend(const uint8_t* buf, int bufLen) {
}

void TcpClient::start() {
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

	this->m_closed = false;
	PollMgr::instance().getEPoll().addFd(this->m_socket.getHandle(),
			Bind(&TcpClient::onSocketUpdate, this));

	this->onConnected();

	PollMgr::instance().getEPoll().addEvents(this->m_socket.getHandle(),
			POLLIN | POLLHUP);
}

void TcpClient::stop() {
	this->m_socket.shutdown();
}

void TcpClient::onSocketUpdate(int fd, int events) {
	if (events & POLLIN) {
		int readBytes = this->m_socket.read(m_readBuffer.getData(),
				m_readBuffer.getSize());
		if (readBytes <= 0) {
			this->m_socket.shutdown();
		} else {
			this->onRecv((const uint8_t*) m_readBuffer.getData(), readBytes);
		}
	}

	if (events & POLLOUT) {
		BufferPtr buffer;
		{
			lock_guard<mutex> lk(this->m_sendMutex);
			if (!this->m_sendBuffers.empty()) {
				buffer = this->m_sendBuffers.front();
				this->m_sendBuffers.pop_front();
			} else {
				PollMgr::instance().getEPoll().delEvents(
						this->m_socket.getHandle(),
						POLLOUT);
			}
		}

		if (buffer) {
			this->m_socket.send(buffer->getData(), buffer->getSize());
			this->onSend(buffer->getData(), buffer->getSize());
		}
	}

	if (events & POLLHUP) {
		PollMgr::instance().getEPoll().delFd(this->m_socket.getHandle());
		this->m_socket.close();
		this->m_closed = true;
		this->onDisconnected();
	}
}

void TcpClient::send(const void *buf, u32 bufSize) {
	if (this->m_closed)
		return;

	lock_guard<mutex> lk(this->m_sendMutex);
	this->m_sendBuffers.push_back(BufferPtr(new Buffer(buf, bufSize)));

	if (1 == this->m_sendBuffers.size())
		PollMgr::instance().getEPoll().addEvents(this->m_socket.getHandle(),
		POLLOUT);
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

} /* namespace NS_FF */
