/*
 * EPoll.h
 *
 *  Created on: Jan 10, 2020
 *      Author: liyawu
 */

#ifndef FF_EPOLL_H_
#define FF_EPOLL_H_

#ifndef _WIN32

#include <ff/ff_config.h>
#include <poll.h>
#include <sys/epoll.h>

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

NS_FF_BEG

typedef std::function<void(int, int)> FdUpdateFunc;

class LIBFF_API EPoll {
 public:
  EPoll(uint32_t maxEpollEvents = 1000);
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
  std::vector<epoll_event> epollEvents;
  // bool m_fdChanged;
  struct FdInfo {
    FdUpdateFunc updateFunc;
    int fd;
    int events;
  };
  typedef std::map<int, FdInfo*> FdInfos;
  FdInfos m_fdInfos;

  std::mutex m_fdInfosMutex;
  std::mutex m_signalMutex;

  bool initSignalPipe();
  void uninitSignalPipe();

  bool addFd2Poll(int fd, void* userData);
  bool delFdFromPoll(int fd);
  bool setEvent2Fd(int fd, int events);

  void onPipeEvents(int fd, int events);
  int doPoll(int timeout);
};

class LIBFF_API PollMgr {
 public:
  PollMgr();
  ~PollMgr();

  static PollMgr& instance() {
    static PollMgr g_pollMgr;
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [&] { g_pollMgr.start(); });
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

using EPollPtr = std::shared_ptr<EPoll>;

NS_FF_END

#endif
#endif /* FF_EPOLL_H_ */
