/*
 * EPoll.cpp
 *
 *  Created on: Jan 10, 2020
 *      Author: liyawu
 */

#ifndef _WIN32

#include <errno.h>
#include <fcntl.h>
#include <ff/Bind.h>
#include <ff/EPoll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>

using namespace std;

NS_FF_BEG

EPoll::EPoll(uint32_t maxEpollEvents)
    : m_epFd(epoll_create1(0)),
      epollEvents(maxEpollEvents)
// m_fdChanged(false)
{
  // this->initSignalPipe();

  // this->addFd(this->m_signalPipe[0], Bind(&EPoll::onPipeEvents, this));
  // this->addEvents(this->m_signalPipe[0], POLLIN);
}

EPoll::~EPoll() {
  if (this->m_epFd >= 0) ::close(this->m_epFd);
}

bool EPoll::initSignalPipe() {
  this->m_signalPipe[0] = -1;
  this->m_signalPipe[1] = -1;

  if (0 != pipe(this->m_signalPipe) ||
      -1 == fcntl(this->m_signalPipe[0], F_SETFL, O_NONBLOCK) ||
      -1 == fcntl(this->m_signalPipe[1], F_SETFL, O_NONBLOCK)) {
    return false;
  }
  return true;
}

bool EPoll::addFd(int fd, const FdUpdateFunc& updateFunc) {
  if (fd <= 0) return false;

  FdInfo* fdInfo = new FdInfo;
  fdInfo->fd = fd;
  fdInfo->updateFunc = updateFunc;
  fdInfo->events = 0;

  {
    lock_guard<mutex> lk(this->m_fdInfosMutex);
    this->m_fdInfos.erase(fd);
    if (!this->m_fdInfos.insert(make_pair(fd, fdInfo)).second) {
      return false;
    }

    if (!this->addFd2Poll(fd, fdInfo)) return false;
    // this->m_fdChanged = true;
  }

  this->signal();
  return true;
}

bool EPoll::delFd(int fd) {
  if (fd <= 0) return false;

  lock_guard<mutex> lk(this->m_fdInfosMutex);
  auto it = this->m_fdInfos.find(fd);
  if (it == this->m_fdInfos.end()) return false;

  delete it->second;
  this->m_fdInfos.erase(it);

  if (!this->delFdFromPoll(fd)) {
    cerr << "del fd[" << fd << "] failed" << endl;
  }

  // m_fdChanged = true;
  this->signal();

  return true;
}

bool EPoll::addEvents(int fd, int events) {
  lock_guard<mutex> lk(this->m_fdInfosMutex);

  auto it = this->m_fdInfos.find(fd);
  if (it == this->m_fdInfos.end()) return false;

  it->second->events |= events;
  this->setEvent2Fd(fd, it->second->events);

  // m_fdChanged = true;
  this->signal();

  return true;
}

bool EPoll::delEvents(int fd, int events) {
  lock_guard<mutex> lk(this->m_fdInfosMutex);

  auto it = this->m_fdInfos.find(fd);
  if (it == this->m_fdInfos.end()) return false;

  it->second->events &= ~events;
  this->setEvent2Fd(fd, it->second->events);

  // this->m_fdChanged = true;
  this->signal();

  return true;
}

bool EPoll::addFd2Poll(int fd, void* userData) {
  struct epoll_event ev;
  ev.events = 0;
  // ev.data.fd = fd;
  ev.data.ptr = userData;

  return (0 == ::epoll_ctl(this->m_epFd, EPOLL_CTL_ADD, fd, &ev));
}

bool EPoll::delFdFromPoll(int fd) {
  return (0 == ::epoll_ctl(this->m_epFd, EPOLL_CTL_DEL, fd, nullptr));
}

bool EPoll::setEvent2Fd(int fd, int events) {
  auto it = this->m_fdInfos.find(fd);

  struct epoll_event ev;
  ev.events = events;
  ev.data.ptr = it->second;

  return (0 == ::epoll_ctl(this->m_epFd, EPOLL_CTL_MOD, fd, &ev));
}

void EPoll::signal() {
  // unique_lock<mutex> lk(this->m_signalMutex, defer_lock);
  // if (!lk.try_lock()) return;
  // char b = 1;
  // ::write(this->m_signalPipe[1], &b, 1);
}

int EPoll::doPoll(int timeout) {
  int fdCnt =
      ::epoll_wait(this->m_epFd, &epollEvents[0], epollEvents.size(), timeout);

  if (fdCnt < 0) return (EINTR == errno ? 0 : -1);
  return fdCnt;
}

void EPoll::update(int pollTimeout) {
  int activeFds;
  if ((activeFds = doPoll(pollTimeout)) < 0) return;

  for (int i = 0; i < activeFds; ++i) {
    auto& pfd = epollEvents[i];
    FdInfo* fdInfo = (FdInfo*)pfd.data.ptr;
    assert(nullptr != fdInfo);

    if (0 == pfd.events) continue;

    if (fdInfo->updateFunc)
      fdInfo->updateFunc(fdInfo->fd, pfd.events & (fdInfo->events | POLLERR |
                                                   POLLHUP | POLLNVAL));
  }
}

void EPoll::onPipeEvents(int fd, int events) {
  if (events & POLLIN) {
    char b;
    read(fd, &b, 1);
  }
}

PollMgr::PollMgr() : m_stoped(true) {}

PollMgr::~PollMgr() { this->stop(); }

EPoll& PollMgr::getEPoll() { return this->m_ep; }

void PollMgr::start() {
  if (!this->m_stoped) return;
  this->m_stoped = false;
  this->m_pollThread = thread(&PollMgr::pollThreadFunc, this);
}

void PollMgr::stop() {
  if (this->m_stoped) return;

  this->m_stoped = true;
  if (this_thread::get_id() != this->m_pollThread.get_id()) {
    this->m_pollThread.join();
  }
}

void PollMgr::pollThreadFunc() {
  while (!this->m_stoped) {
    this->m_ep.update(100);
  }
}

NS_FF_END
#endif
