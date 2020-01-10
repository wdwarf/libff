/*
 * EPoll.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: u16
 */

#include "EPoll.h"
#include <cstring>
#include <memory>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

using namespace std;
using namespace std::placeholders;

namespace NS_FF {

EPoll::EPoll() :
		m_epFd(epoll_create1(0)), m_fdChanged(false) {
	this->initSignalPipe();

	this->addFd(this->m_signalPipe[0], bind(&EPoll::onPipeEvents, this, _1));
	this->addEvents(this->m_signalPipe[0], POLLIN);
}

EPoll::~EPoll() {
	if (this->m_epFd >= 0)
		::close(this->m_epFd);
}

bool EPoll::initSignalPipe() {
	this->m_signalPipe[0] = -1;
	this->m_signalPipe[1] = -1;

	if (0 != pipe(this->m_signalPipe)
			|| -1 == fcntl(this->m_signalPipe[0], F_SETFL, O_NONBLOCK)
			|| -1 == fcntl(this->m_signalPipe[1], F_SETFL, O_NONBLOCK)) {
		return false;
	}
	return true;
}

bool EPoll::addFd(int fd, const FdUpdateFunc& updateFunc) {
	if (fd < 0)
		return false;

	FdInfo fdInfo;
	fdInfo.fd = fd;
	fdInfo.updateFunc = updateFunc;
	fdInfo.events = 0;

	{
		lock_guard < mutex > lk(this->m_fdInfosMutex);
		if (!this->m_fdInfos.insert(make_pair(fd, fdInfo)).second) {
			return false;
		}

		this->addFd2Poll(fd);
		this->m_fdChanged = true;
	}

	this->signal();
	return true;
}

bool EPoll::delFd(int fd) {
	if (fd < 0)
		return false;

	lock_guard < mutex > lk(this->m_fdInfosMutex);
	auto it = this->m_fdInfos.find(fd);
	if (it == this->m_fdInfos.end())
		return false;

	this->m_fdInfos.erase(it);

	{
		lock_guard < mutex > lk(this->m_justDeletedMutex);
		this->m_justDeletedFds.push_back(fd);
	}

	this->delFdFromPoll(fd);

	m_fdChanged = true;
	this->signal();

	return true;
}

bool EPoll::addEvents(int fd, int events) {
	lock_guard < mutex > lk(this->m_fdInfosMutex);

	auto it = this->m_fdInfos.find(fd);

	if (it == this->m_fdInfos.end()) {
		return false;
	}

	it->second.events |= events;
	this->setEvent2Fd(fd, it->second.events);

	m_fdChanged = true;
	this->signal();

	return true;
}

bool EPoll::delEvents(int fd, int events) {
	lock_guard < mutex > lk(this->m_fdInfosMutex);

	auto it = this->m_fdInfos.find(fd);
	if (it == this->m_fdInfos.end())
		return false;

	it->second.events &= ~events;
	this->setEvent2Fd(fd, it->second.events);

	this->m_fdChanged = true;
	this->signal();

	return true;
}

bool EPoll::addFd2Poll(int fd) {
	struct epoll_event ev;
	bzero(&ev, sizeof(ev));

	ev.events = 0;
	ev.data.fd = fd;

	return (0 == ::epoll_ctl(this->m_epFd, EPOLL_CTL_ADD, fd, &ev));
}

bool EPoll::delFdFromPoll(int fd) {
	return (0 == ::epoll_ctl(this->m_epFd, EPOLL_CTL_DEL, fd, nullptr));
}

void EPoll::signal() {
	unique_lock < mutex > lk(this->m_signalMutex, defer_lock);
	if (!lk.try_lock())
		return;
	char b = true;
	::write(this->m_signalPipe[1], &b, 1);
}

bool EPoll::setEvent2Fd(int fd, int events) {
	struct epoll_event ev;
	bzero(&ev, sizeof(ev));

	ev.events = events;
	ev.data.fd = fd;

	return (0 == ::epoll_ctl(this->m_epFd, EPOLL_CTL_MOD, fd, &ev));
}

void EPoll::createNativePolls() {
	lock_guard < mutex > lk(this->m_fdInfosMutex);

	if (!this->m_fdChanged)
		return;

	this->m_pollfds.resize(this->m_fdInfos.size());
	int i = 0;
	for (auto p : this->m_fdInfos) {
		const FdInfo& info = p.second;
		pollfd& pfd = this->m_pollfds[i];
		pfd.fd = info.fd;
		pfd.events = info.events;
		pfd.revents = 0;
		++i;
	}
	this->m_fdChanged = false;
}

bool EPoll::doPoll(std::vector<pollfd>& ofds, int epFd, pollfd *fds,
		nfds_t nfds, int timeout) {
	struct epoll_event ev[nfds];

	int fdCnt = ::epoll_wait(epFd, ev, nfds, timeout);

	if (fdCnt < 0)
		return (EINTR == errno);

	for (int i = 0; i < fdCnt; i++) {
		pollfd pfd;
		pfd.fd = ev[i].data.fd;
		pfd.revents = ev[i].events;
		ofds.push_back(pfd);
	}
	return true;
}

void EPoll::update(int pollTimeout) {
	this->createNativePolls();

	vector<pollfd> ofds;
	if (!doPoll(ofds, this->m_epFd, &this->m_pollfds.front(),
			this->m_pollfds.size(), pollTimeout))
		return;

	for (auto pfd : ofds) {
		int fd = pfd.fd;
		int revents = pfd.revents;
		FdUpdateFunc updateFunc;
		int events = 0;

		if (0 == revents)
			continue;

		{
			lock_guard < mutex > lk(this->m_fdInfosMutex);
			auto it = this->m_fdInfos.find(fd);

			if (it == this->m_fdInfos.end())
				continue;

			const FdInfo& info = it->second;

			updateFunc = info.updateFunc;
			events = info.events;
		}

		if (updateFunc
				&& ((events & revents) || (revents & POLLERR)
						|| (revents & POLLHUP) || (revents & POLLNVAL))) {
			bool skip = false;
			if (revents & (POLLNVAL | POLLERR | POLLHUP)) {
				lock_guard < mutex > lk(this->m_justDeletedMutex);
				if (std::find(this->m_justDeletedFds.begin(),
						this->m_justDeletedFds.end(), fd)
						!= this->m_justDeletedFds.end()) {
					skip = true;
				}
			}

			if (!skip) {
				updateFunc(revents & (events | POLLERR | POLLHUP | POLLNVAL));
			}
		}
	}

	lock_guard < mutex > lk(this->m_justDeletedMutex);
	this->m_justDeletedFds.clear();

}

void EPoll::onPipeEvents(int events) {

}

} /* namespace NS_FF */
