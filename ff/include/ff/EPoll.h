/*
 * EPoll.h
 *
 *  Created on: Jan 10, 2020
 *      Author: liyawu
 */

#ifndef FF_EPOLL_H_
#define FF_EPOLL_H_

#include <functional>
#include <mutex>
#include <map>
#include <list>
#include <vector>
#include <thread>
#include <poll.h>
#include <ff/ff_config.h>

namespace NS_FF {

typedef std::function<void(int, int)> FdUpdateFunc;

struct SignalInfo{
	uint32_t serialNum;
	uint64_t sigId;
	uint64_t sigEvent;

	SignalInfo();
	SignalInfo(uint64_t sigId, uint64_t sigEvent);
};

class EPoll {
public:
	EPoll();
	virtual ~EPoll();

	bool addFd(int fd, const FdUpdateFunc& updateFunc);
	bool delFd(int fd);
	bool addEvents(int fd, int events);
	bool delEvents(int fd, int events);
	void update(int pollTimeout);

	uint32_t signal(uint64_t sigId = 0, uint64_t sigEvent = 0);
	void signalRsp(uint32_t serialNum, uint64_t sigRspId = 0, uint64_t sigEvent = 0);
protected:
	int m_epFd;
	int m_signalPipe[2];
	bool m_fdChanged;
	struct FdInfo {
		FdUpdateFunc updateFunc;
		int fd;
		int events;
	};
	typedef std::map<int, FdInfo> FdInfos;
	FdInfos m_fdInfos;
	std::list<int> m_justDeletedFds;
	std::vector<pollfd> m_pollfds;

	std::mutex m_fdInfosMutex;
	std::mutex m_signalMutex;
	std::mutex m_justDeletedMutex;

	bool initSignalPipe();
	void uninitSignalPipe();

	bool addFd2Poll(int fd);
	bool delFdFromPoll(int fd);
	bool setEvent2Fd(int fd, int events);

	void createNativePolls();
	virtual void onSignal(const SignalInfo& sig){}
	virtual void onSignalRsp(const SignalInfo& sigRsp){}
	void onSignalEvents(int fd, int events);
	void onSignalRspEvents(int fd, int events);
	bool doPoll(std::vector<pollfd>& ofds, int epFd, pollfd *fds, nfds_t nfds,
			int timeout);
};

class PollMgr {
public:
	PollMgr();
	~PollMgr();

	static PollMgr& instance() {
		static PollMgr g_pollMgr;
		static std::once_flag onceFlag;
		std::call_once(onceFlag, [&] {g_pollMgr.start();});
		return g_pollMgr;
	}

	void start();
	void stop();

	EPoll& getEPoll();

private:
	EPoll m_ep;
	bool m_stoped;
	std::thread m_pollThread;
	void pollThreadFunc();
};

} /* namespace NS_FF */

#endif /* FF_EPOLL_H_ */