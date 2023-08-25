/**
 * @file TimingWheel.h
 * @author liyawu
 * @date 2023-08-24 09:42:19
 * @description
 */

#ifndef _TIMINGWHEEL_H
#define _TIMINGWHEEL_H

#include <ff/Tick.h>
#include <ff/ff_config.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

NS_FF_BEG

using TimingTaskFunc = std::function<void()>;

class TimingWheel;

class LIBFF_API TimingTask {
 public:
  TimingTask(TimingTaskFunc func, uint64_t round);
  uint64_t round() const;
  void round(uint64_t r);
  TimingTaskFunc taskFunc() const;
  void cancle() { m_cancled = true; }
  bool isCancled() { return m_cancled; }

 private:
  uint64_t m_round = 0;
  std::atomic_bool m_cancled{false};
  TimingTaskFunc m_tskFunc;
  friend class TimingWheel;
};

using TimingTaskPtr = std::shared_ptr<TimingTask>;
using TimingTaskList = std::set<TimingTaskPtr>;

class LIBFF_API TimingWheelNode {
 public:
  TimingTaskList& taskList() { return m_taskList; }

 private:
  TimingTaskList m_taskList;
};

using TimingTicker = ff::Tick;

class LIBFF_API TimingWheel {
 public:
  TimingWheel();
  ~TimingWheel();

  TimingTaskPtr addTask(TimingTaskFunc taskFunc, uint64_t delayMs);

 private:
  uint64_t m_startTick;
  uint64_t m_currentPos;
  std::vector<TimingWheelNode> m_wheel;
  std::atomic_bool m_stoped;
  std::atomic_bool m_ready{false};
  std::thread m_timingThread;
  std::mutex m_mutex;
  std::condition_variable m_cond;
  TimingTicker m_ticker;
  uint64_t m_lastTock = 0;
  void run();
  void doTick(uint64_t tickPassed);
  uint64_t getLastMs();

  std::vector<std::thread> m_execThreads;
  std::mutex m_mutexExec;
  std::condition_variable m_condExec;
  std::list<TimingTaskFunc> m_expiredTaskFuncs;
};

NS_FF_END

#endif  // _TIMINGWHEEL_H
