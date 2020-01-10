/*
 * EPoll.h
 *
 *  Created on: Jan 10, 2020
 *      Author: u16
 */

#ifndef EPOLL_H_
#define EPOLL_H_

#include <functional>
#include <mutex>
#include <map>
#include <list>
#include <vector>
#include <poll.h>

namespace NS_FF {

typedef std::function<void(int)> FdUpdateFunc;

class EPoll {
public:
	EPoll();
	virtual ~EPoll();

	bool addFd(int fd, const FdUpdateFunc& updateFunc);
	bool delFd(int fd);
	bool addEvents(int fd, int events);
	bool delEvents(int fd, int events);

private:
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
	void signal();
	bool addFd2Poll(int fd);
	bool delFdFromPoll(int fd);
	bool setEvent2Fd(int fd, int events);

	void createNativePolls();
	void update(int pollTimeout);
	void onPipeEvents(int events);
	bool doPoll(std::vector<pollfd>& ofds, int epFd,
			pollfd *fds, nfds_t nfds, int timeout);
};

} /* namespace NS_FF */

#endif /* EPOLL_H_ */
