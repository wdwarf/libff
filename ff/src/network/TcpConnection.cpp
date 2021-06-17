/*
 * TcpConnection.cpp
 *
 *  Created on: Jan 17, 2020
 *      Author: liyawu
 */

#include <iostream>
#include <cstring>
#include <errno.h>
#include <ff/Bind.h>
#include <ff/EPoll.h>
#include <ff/StringWrapper.h>
#include <ff/Exception.h>
#include <ff/TcpConnection.h>
#include <ff/Bind.h>

using namespace std;

#define RD_BUF_SIZE 4096

NS_FF_BEG

#ifdef _WIN32

	// TcpConnection::TcpConnection() :
	// 	m_isServer(false), m_readBuffer(RD_BUF_SIZE) {
	// 	this->m_socket.setUseSelect(false);
	// }

	TcpConnection::TcpConnection(IOCPPtr iocp) :
		m_isServer(false), m_readBuffer(RD_BUF_SIZE), m_iocp(iocp) {
		this->m_socket.setUseSelect(false);
	}

	TcpConnection::TcpConnection(Socket&& socket, IOCPPtr iocp) :
		m_isServer(false), m_socket(std::move(socket)), m_readBuffer(RD_BUF_SIZE) {
		this->m_socket.setUseSelect(false);
		this->m_socket.setBlocking(true);

		this->m_iocp = iocp;
		this->m_iocp->connect((HANDLE)this->m_socket.getHandle(), (ULONG_PTR)&m_context,
			Bind(&TcpConnection::workThreadFunc, this));
	}

	TcpConnectionPtr TcpConnection::CreateInstance(IOCPPtr iocp){
		return TcpConnectionPtr(new TcpConnection(iocp));
	}

	TcpConnection::~TcpConnection() {
		this->m_socket.close();
		if (this->m_isServer) {
			this->m_acceptThread.join();
		}
	}

	bool TcpConnection::isServer() const {
		return this->m_isServer;
	}

	void TcpConnection::resetCallbackFunctions() {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onDataFunc = nullptr;
		this->m_onCloseFunc = nullptr;
		this->m_onAcceptFunc = nullptr;
		this->m_sendBuffers.clear();
	}

	void TcpConnection::workThreadFunc(LPDWORD lpNumberOfBytesTransferred,
		PULONG_PTR lpCompletionKey,
		LPOVERLAPPED* lpOverlapped)
	{
		PIocpContext context = (PIocpContext)*lpOverlapped;
		if (nullptr == context) return;

		switch (context->iocpEevent) {
		case IocpEvent::Recv:
		{
			if (0 == *lpNumberOfBytesTransferred)
			{
				auto pThis = this->shared_from_this();
				OnCloseFunc func;
				{
					lock_guard<mutex> lk(this->m_mutex);
					func = this->m_onCloseFunc;
				}
				if (func)
					func(pThis);

				this->m_socket.close();

				break;
			}

			DWORD flags = MSG_PARTIAL;
			DWORD numToRecvd = 0;

			OnDataFunc func;
			{
				lock_guard<mutex> lk(this->m_mutex);
				func = this->m_onDataFunc;
			}
			if (func)
				func((const uint8_t*)context->buffer.buf, *lpNumberOfBytesTransferred,
					this->shared_from_this());

			int ret = WSARecv(this->m_socket.getHandle(),
				&m_context.buffer,
				1,
				&numToRecvd,
				&flags,
				&m_context,
				NULL);
			if (SOCKET_ERROR == ret)
			{
				if (WSA_IO_PENDING != WSAGetLastError())
				{
					OnCloseFunc func;
					{
						lock_guard<mutex> lk(this->m_mutex);
						func = this->m_onCloseFunc;
					}
					if (func)
						func(this->shared_from_this());

					this->m_socket.close();
				}
			}

			break;
		}
		case IocpEvent::Send:
			break;
		}

	}

	bool TcpConnection::listen(uint16_t port, const std::string& ip,
		IpVersion ipVer, int backlog) {
		// this->resetCallbackFunctions();

		try {
			if (!this->m_socket.createTcp(ipVer))
				THROW_EXCEPTION(Exception, "Create socket failed.", errno);
			if (!this->m_socket.bind(port, ip))
				THROW_EXCEPTION(Exception,
					SW("Bind to ")(port)(":")(ip)(" failed. ")(strerror(errno)),
					errno);
			this->m_socket.listen(backlog);
		}
		catch (std::exception& e) {
			this->m_socket.close();
			return false;
		}

		this->m_isServer = true;

		/** TODO start accept thread */
		this->m_acceptThread = thread([this] {
			SockAddr addr;
			while (this->m_socket.getHandle() > 0) {
				Socket client = this->m_socket.accept(addr);
				if (client.getHandle() <= 0 || this->m_socket.getHandle() <= 0) break;
				TcpConnectionPtr tcpSock = TcpConnectionPtr(
					new TcpConnection(move(client)));

				OnAcceptFunc func;
				{
					lock_guard<mutex> lk(this->m_mutex);
					func = m_onAcceptFunc;
				}
				if (func)
					func(tcpSock);
			}
			});

			return true;
	}

	bool TcpConnection::connect(uint16_t remotePort, const std::string& remoteHost,
		uint16_t localPort, const std::string& localIp) {
		// this->resetCallbackFunctions();

		this->m_isServer = false;
		try {
			IpVersion ipVer = IpVersion::V4;
			SockAddr addr(remoteHost, remotePort);
			if (addr.isValid()) {
				ipVer = addr.getVersion();
			}

			if (!this->m_socket.createTcp(ipVer))
				THROW_EXCEPTION(Exception, "Create socket failed.", errno);

			if (localPort > 0) {
				if (!this->m_socket.bind(localPort, localIp))
					THROW_EXCEPTION(Exception,
						SW("Bind to ")(localPort)(":")(localIp)(" failed. ")(strerror(errno)),
						errno);
			}

			if (!this->m_socket.connect(remoteHost, remotePort, 5000)) {
				THROW_EXCEPTION(Exception,
					SW("Connect to ")(remoteHost)(":")(remotePort)(" failed.")(strerror(errno)),
					errno);
			}

			this->m_socket.setBlocking(true);

		}
		catch (std::exception& e) {
			this->m_socket.close();
			return false;
		}

		/** TODO add to iocp */
		this->m_iocp->connect((HANDLE)this->m_socket.getHandle(), this->m_socket.getHandle(),
			Bind(&TcpConnection::workThreadFunc, this));
		return true;
	}

	void TcpConnection::close() {
		this->m_socket.shutdown();

		OnCloseFunc func;
		{
			lock_guard<mutex> lk(this->m_mutex);
			func = this->m_onCloseFunc;
		}
		if (!func) {
			this->m_socket.close();
		}
	}

	Socket& TcpConnection::getSocket() {
		return this->m_socket;
	}

	void TcpConnection::send(const void* buf, uint32_t bufSize) {
		if (this->m_isServer)
			return;
		this->m_socket.send(buf, bufSize);
	}

	TcpConnection& TcpConnection::onAccept(const OnAcceptFunc& func) {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onAcceptFunc = func;
		return *this;
	}

	TcpConnection& TcpConnection::onData(const OnDataFunc& func) {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onDataFunc = func;

		DWORD flags = MSG_PARTIAL;
		DWORD numToRecvd = 0;

		m_context.handle = (HANDLE)this->m_socket.getHandle();
		m_context.iocpEevent = IocpEvent::Recv;
		m_context.buffer.buf = (char*)this->m_readBuffer.getData();
		m_context.buffer.len = this->m_readBuffer.getSize();

		int ret = WSARecv(this->m_socket.getHandle(),
			&m_context.buffer,
			1,
			&numToRecvd,
			&flags,
			&m_context,
			NULL);

		return *this;
	}

	TcpConnection& TcpConnection::onClose(const OnCloseFunc& func) {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onCloseFunc = func;
		return *this;
	}

#else

	TcpConnection::TcpConnection() :
		m_isServer(false), m_readBuffer(RD_BUF_SIZE) {
		this->m_socket.setUseSelect(false);
		this->m_ep = &PollMgr::instance().getEPoll();
	}

	TcpConnection::TcpConnection(Socket&& socket) :
		m_isServer(false), m_socket(std::move(socket)), m_readBuffer(RD_BUF_SIZE) {
		this->m_socket.setUseSelect(false);
		this->m_ep = &PollMgr::instance().getEPoll();
		this->m_ep->addFd(this->m_socket.getHandle(),
			Bind(&TcpConnection::onSocketUpdate, this));
	}

	TcpConnection::~TcpConnection() {
		this->m_ep->delFd(this->m_socket.getHandle());
		this->m_socket.close();
	}

	bool TcpConnection::isServer() const {
		return this->m_isServer;
	}

	void TcpConnection::resetCallbackFunctions() {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onDataFunc = nullptr;
		this->m_onCloseFunc = nullptr;
		this->m_onAcceptFunc = nullptr;
		this->m_sendBuffers.clear();
	}

	bool TcpConnection::listen(uint16_t port, const std::string& ip,
		IpVersion ipVer, int backlog) {
		this->resetCallbackFunctions();

		try {
			if (!this->m_socket.createTcp(ipVer))
				THROW_EXCEPTION(Exception, "Create socket failed.", errno);
			if (!this->m_socket.bind(port, ip))
				THROW_EXCEPTION(Exception,
					SW("Bind to ")(port)(":")(ip)(" failed. ")(strerror(errno)),
					errno);
			this->m_socket.listen(backlog);
		}
		catch (std::exception& e) {
			this->m_socket.close();
			return false;
		}

		this->m_isServer = true;
		this->m_ep->addFd(this->m_socket.getHandle(),
			Bind(&TcpConnection::onSocketUpdate, this));
		return true;
	}

	bool TcpConnection::connect(uint16_t remotePort, const std::string& remoteHost,
		uint16_t localPort, const std::string& localIp) {
		this->resetCallbackFunctions();

		this->m_isServer = false;
		try {
			IpVersion ipVer = IpVersion::V4;
			SockAddr addr(remoteHost, remotePort);
			if (addr.isValid()) {
				ipVer = addr.getVersion();
			}

			if (this->m_socket.createTcp(ipVer) <= 0)
				THROW_EXCEPTION(Exception, "Create socket failed.", errno);

			if (localPort > 0) {
				if (!this->m_socket.bind(localPort, localIp))
					THROW_EXCEPTION(Exception,
						SW("Bind to ")(localPort)(":")(localIp)(" failed. ")(strerror(errno)),
						errno);
			}

			if (!this->m_socket.connect(remoteHost, remotePort, 5000)) {
				THROW_EXCEPTION(Exception,
					SW("Connect to ")(remoteHost)(":")(remotePort)(" failed.")(strerror(errno)),
					errno);
			}

		}
		catch (std::exception& e) {
			this->m_socket.close();
			return false;
		}

		this->m_ep->addFd(this->m_socket.getHandle(),
			Bind(&TcpConnection::onSocketUpdate, this));
		return true;
	}

	void TcpConnection::close() {
		this->m_socket.shutdown();

		OnCloseFunc func;
		{
			lock_guard<mutex> lk(this->m_mutex);
			func = this->m_onCloseFunc;
		}
		if (!func) {
			this->m_ep->delFd(this->m_socket.getHandle());
			this->m_socket.close();
		}
	}

	Socket& TcpConnection::getSocket() {
		return this->m_socket;
	}

	void TcpConnection::send(const void* buf, uint32_t bufSize) {
		if (this->m_isServer)
			return;

		lock_guard<mutex> lk(this->m_sendMutex);
		this->m_sendBuffers.push_back(BufferPtr(new Buffer(buf, bufSize)));

		if (1 == this->m_sendBuffers.size())
			this->m_ep->addEvents(this->m_socket.getHandle(),
				POLLOUT);
	}

	TcpConnection& TcpConnection::onAccept(const OnAcceptFunc& func) {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onAcceptFunc = func;
		this->m_ep->addEvents(this->m_socket.getHandle(),
			POLLIN);
		return *this;
	}

	TcpConnection& TcpConnection::onData(const OnDataFunc& func) {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onDataFunc = func;
		this->m_ep->addEvents(this->m_socket.getHandle(),
			POLLIN);
		return *this;
	}

	TcpConnection& TcpConnection::onClose(const OnCloseFunc& func) {
		lock_guard<mutex> lk(this->m_mutex);
		this->m_onCloseFunc = func;
		this->m_ep->addEvents(this->m_socket.getHandle(),
			POLLHUP);
		return *this;
	}

	void TcpConnection::onSvrSocketUpdate(int fd, int events) {
		if (events & POLLIN) {
			SockAddr addr;
			auto sock = this->m_socket.accept(addr);
			TcpConnectionPtr tcpSock = TcpConnectionPtr(
				new TcpConnection(move(sock)));

			OnAcceptFunc func;
			{
				lock_guard<mutex> lk(this->m_mutex);
				func = m_onAcceptFunc;
			}
			if (func)
				func(tcpSock);
		}

		if (events & POLLHUP) {
			this->m_ep->delFd(this->m_socket.getHandle());
			this->m_socket.close();

			OnCloseFunc func;
			{
				lock_guard<mutex> lk(this->m_mutex);
				func = m_onCloseFunc;
			}
			if (func)
				func(this->shared_from_this());
		}
	}

	void TcpConnection::onClientSocketUpdate(int fd, int events) {
		if (events & POLLIN) {
			int readBytes = this->m_socket.read(m_readBuffer.getData(),
				m_readBuffer.getSize());
			if (readBytes <= 0) {
				this->m_socket.shutdown();
			}
			else {
				OnDataFunc func;
				{
					lock_guard<mutex> lk(this->m_mutex);
					func = this->m_onDataFunc;
				}
				if (func)
					func((const uint8_t*)m_readBuffer.getData(), readBytes,
						this->shared_from_this());
			}
		}

		if (events & POLLOUT) {
			BufferPtr buffer;
			{
				lock_guard<mutex> lk(this->m_sendMutex);
				if (!this->m_sendBuffers.empty()) {
					buffer = this->m_sendBuffers.front();
					this->m_sendBuffers.pop_front();
				}
				else {
					this->m_ep->delEvents(this->m_socket.getHandle(), POLLOUT);
				}
			}

			if (buffer) {
				this->m_socket.send(buffer->getData(), buffer->getSize());
			}
		}

		if (events & POLLHUP) {
			this->m_ep->delFd(this->m_socket.getHandle());
			this->m_socket.close();

			OnCloseFunc func;
			{
				lock_guard<mutex> lk(this->m_mutex);
				func = m_onCloseFunc;
			}
			if (func)
				func(this->shared_from_this());
		}
	}

	void TcpConnection::onSocketUpdate(int fd, int events) {
		(this->m_isServer ?
			this->onSvrSocketUpdate(fd, events) :
			this->onClientSocketUpdate(fd, events));
	}

	TcpConnectionPtr TcpConnection::CreateInstance() {
		return TcpConnectionPtr(new TcpConnection);
	}

#endif

NS_FF_END
