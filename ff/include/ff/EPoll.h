/*
 * EPoll.h
 *
 *  Created on: Jan 10, 2020
 *      Author: liyawu
 */

#ifndef FF_EPOLL_H_
#define FF_EPOLL_H_

#ifndef _WIN32

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

class EPoll {
public:
	EPoll();
	virtual ~EPoll();

	bool addFd(int fd, const FdUpdateFunc& updateFunc);
	bool delFd(int fd);
	bool addEvents(int fd, int events);
	bool delEvents(int fd, int events);
	void update(int pollTimeout);

	void signal();
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
	void onPipeEvents(int fd, int events);
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

#endif
#endif /* FF_EPOLL_H_ */
