/**
 * @file Timer.h
 * @auth DuckyLi
 * @date 2022-05-06 11:35:46
 * @description
 */

#ifndef _FF_TIMER_H_
#define _FF_TIMER_H_

#include <ff/ThreadPool.h>
#include <ff/Tick.h>
#include <ff/ff_config.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

NS_FF_BEG

using TimerID = uint64_t;
using TimerFunc = std::function<void()>;

enum class TimerTaskType : uint8_t { Once = 0, Repeat = 1 };

struct TimerTask {
  TimerTask* next;
  TimerTask* prev;
  TimerID id;
  TimerTaskType type;
  uint32_t timeout;
  uint64_t deadline;
  TimerFunc func;
  bool cancled;

  TimerTask()
      : next(nullptr),
        prev(nullptr),
        id(0),
        type(TimerTaskType::Once),
        timeout(0),
        deadline(0), cancled(false) {}
};

using PTimerTask = TimerTask*;

class LIBFF_API Timer {
 private:
  PTimerTask m_tasks;
  std::thread m_timerThread;
  std::list<PTimerTask> m_insertTasks;
  std::list<PTimerTask> m_reapetTasks;
  std::mutex m_mutex;
  std::mutex m_mutexNewTask;
  std::condition_variable m_cond;
  Tick m_tick;
  ThreadPool m_threadPool;
  std::atomic_bool m_stop;

  void timerThreadFunc();
  int64_t getNextWaitMs();
  void processNewTasks();
  void processNewTask(PTimerTask task);
  void insertTask(PTimerTask task);
  void execTask(PTimerTask task);

 public:
  Timer(uint32_t taskThreadCnt = 2);
  ~Timer();

  TimerID setTimeout(TimerFunc func, uint64_t timeoutMs);
  TimerID setInterval(TimerFunc func, uint64_t intervalMs);

  void cancelTimeout(TimerID id);
};

NS_FF_END

#endif /* _FF_TIMER_H_ */
