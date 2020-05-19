/*
 * IOCP.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#include <ff/windows/IOCP.h>
#include <cstring>

using namespace std;

namespace NS_FF
{
	IocpContext::IocpContext()
	{
		memset(this, 0, sizeof(IocpContext));
	}
	IocpContext::IocpContext(HANDLE handle, IocpEvent event)
	{
		memset(this, 0, sizeof(IocpContext));
		this->handle = handle;
		this->iocpEevent = event;
	}

	IOCP::IOCP(DWORD concurrentThreads) : m_handle(NULL)
	{
		if (concurrentThreads > 0)
		{
			this->create(concurrentThreads);
		}
	}

	IOCP::~IOCP()
	{
		this->close();
	}

	IOCP::operator HANDLE() const
	{
		return this->m_handle;
	}

	IOCP::operator bool() const
	{
		return (NULL != this->m_handle);
	}

	bool IOCP::create(DWORD numberOfConcurrentThreads)
	{
		if (numberOfConcurrentThreads <= 0)
			return false;

		this->close();

		this->m_handle = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE, NULL, NULL, numberOfConcurrentThreads);
		if ((NULL == this->m_handle))
			return false;

		this->m_workThreads = std::vector<std::thread>(numberOfConcurrentThreads);
		for (DWORD i = 0; i < numberOfConcurrentThreads; ++i) {
			this->m_workThreads[i] = thread([this] {
				while (true) {
					DWORD numberOfBytesTransferred = 0;
					ULONG_PTR completionKey = 0;
					LPOVERLAPPED lpOverlapped = nullptr;

					if (!this->getQueuedCompletionStatus(&numberOfBytesTransferred,
						&completionKey, &lpOverlapped, -1)
						|| (0 == completionKey && nullptr == lpOverlapped))
						break;

					
					PIocpContext context = (PIocpContext)lpOverlapped;

					IocpWorkThreadFunc func;
					{
						lock_guard<mutex> lk(this->m_mutex);
						auto it = this->m_iocpWorkThreadFuncs.find(context->handle);
						if (it != this->m_iocpWorkThreadFuncs.end())
							func = it->second;
					}
					if(func)
						func(&numberOfBytesTransferred, &completionKey, &lpOverlapped);
				}
				});
		}

		return (NULL != this->m_handle);
	}

	bool IOCP::connect(HANDLE fileHandle, ULONG_PTR completionKey, IocpWorkThreadFunc iocpWorkThreadFunc)
	{
		HANDLE hCp = CreateIoCompletionPort(
			fileHandle,
			this->m_handle,
			completionKey,
			0);
		if (hCp != this->m_handle) return false;
		lock_guard<mutex> lk(this->m_mutex);
		this->m_iocpWorkThreadFuncs[fileHandle] = iocpWorkThreadFunc;
		return true;
	}

	void IOCP::close()
	{
		if (NULL == this->m_handle)
			return;

		for (auto& t : this->m_workThreads) {
			PostQueuedCompletionStatus(this->m_handle, 0, 0, nullptr);
		};

		for (auto& t : this->m_workThreads) {
			t.join();
		};

		CloseHandle(this->m_handle);
		this->m_handle = NULL;
	}

	bool IOCP::getQueuedCompletionStatus(
		__out LPDWORD lpNumberOfBytesTransferred,
		__out PULONG_PTR lpCompletionKey,
		__out LPOVERLAPPED* lpOverlapped,
		__in DWORD dwMilliseconds)
	{
		return (FALSE != ::GetQueuedCompletionStatus(this->m_handle,
			lpNumberOfBytesTransferred,
			lpCompletionKey,
			lpOverlapped,
			dwMilliseconds));
	}


} // namespace NS_FF