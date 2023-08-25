/**
 * @file TimingWheel.cpp
 * @author liyawu
 * @date 2023-08-24 09:42:31
 * @description
 */
#include <ff/TimingWheel.h>

#include <iostream>

using namespace std;

#define WHEEL_SIZE 100

NS_FF_BEG

TimingTask::TimingTask(TimingTaskFunc func, uint64_t round)
    : m_tskFunc(func), m_round(round) {}

uint64_t TimingTask::round() const { return m_round; }

void TimingTask::round(uint64_t r) { m_round = r; }

TimingTaskFunc TimingTask::taskFunc() const { return m_tskFunc; }

TimingWheel::TimingWheel()
    : m_startTick(0),
      m_currentPos(0),
      m_wheel(WHEEL_SIZE),
      m_stoped(false),
      m_execThreads(16) {
  for (auto& t : m_execThreads) {
    t = std::thread([this] {
      while (!m_stoped) {
        TimingTaskFunc func;
        {
          unique_lock<mutex> lk(m_mutexExec);
          if (m_expiredTaskFuncs.empty()) {
            m_condExec.wait(lk);
          }

          if (m_stoped) break;
          if (m_expiredTaskFuncs.empty()) continue;
          func = m_expiredTaskFuncs.front();
          m_expiredTaskFuncs.pop_front();
        }

        if (func) func();
      }
    });
  }
  m_timingThread = std::thread([this] { run(); });
}

TimingWheel::~TimingWheel() {
  {
    lock_guard<mutex> lkg(m_mutex);
    m_stoped = true;
    m_cond.notify_one();
  }
  if (m_timingThread.joinable()) m_timingThread.join();

  {
    unique_lock<mutex> lk(m_mutexExec);
    m_condExec.notify_all();
  }
  for (auto& t : m_execThreads) {
    if (t.joinable()) t.join();
  }
}

TimingTaskPtr TimingWheel::addTask(TimingTaskFunc taskFunc, uint64_t delayMs) {
  lock_guard<mutex> lkg(m_mutex);
  if (m_stoped) return nullptr;

  // while (!m_stoped && !m_ready) {
  // }

  delayMs += (m_ticker.tock() - m_lastTock);
  auto round = delayMs / WHEEL_SIZE;
  auto pos = (m_currentPos + (delayMs % WHEEL_SIZE)) % WHEEL_SIZE;

  auto& node = m_wheel[pos];

  auto task = std::make_shared<TimingTask>(taskFunc, round);
  node.taskList().insert(task);
  m_cond.notify_one();

  return task;
}

uint64_t TimingWheel::getLastMs() {
  for (int i = 1; i < WHEEL_SIZE; ++i) {
    uint64_t pos = ((m_currentPos + i) % WHEEL_SIZE);
    if (!m_wheel[pos].taskList().empty()) return i;
  }
  return 1000;
}

void TimingWheel::run() {
  m_ticker.tick();
  m_lastTock = m_ticker.tock();
  m_ready = true;
  while (!m_stoped) {
    unique_lock<mutex> lk(m_mutex);
    m_cond.wait_for(lk, chrono::milliseconds(getLastMs()));

    auto tock = m_ticker.tock();
    auto tickPassed = tock - m_lastTock;
    m_lastTock = tock;
    if (0 == tickPassed) continue;
    doTick(tickPassed);
  }
}

void TimingWheel::doTick(uint64_t tickPassed) {
  static uint64_t n = 0;
  static TimingTicker gt;
  n += tickPassed;
  auto round = tickPassed / WHEEL_SIZE;
  auto tick = tickPassed % WHEEL_SIZE;

  if (round > 0) {
    for (uint64_t i = 1; i <= WHEEL_SIZE; ++i) {
      uint64_t pos = ((m_currentPos + i) % WHEEL_SIZE);
      TimingTaskList handledList;
      auto& node = m_wheel[pos];
      auto& taskList = node.taskList();
      if (taskList.empty()) continue;
      for (auto task : taskList) {
        if (task->isCancled()) {
          handledList.insert(task);
          continue;
        }

        if (0 == task->round()) {
          unique_lock<mutex> lk(m_mutexExec);
          m_expiredTaskFuncs.push_back(task->taskFunc());
          m_condExec.notify_one();
          handledList.insert(task);
          continue;
        }

        if (round > task->round()) {
          task->round(0);
          unique_lock<mutex> lk(m_mutexExec);
          m_expiredTaskFuncs.push_back(task->taskFunc());
          m_condExec.notify_one();
          handledList.insert(task);
        } else {
          task->round(task->round() - round);
        }
      }
      for (auto& task : handledList) {
        taskList.erase(task);
      }
    }
  }

  uint64_t pos = m_currentPos;
  for (uint64_t i = 1; i <= tick; ++i) {
    pos = (m_currentPos + i) % WHEEL_SIZE;
    TimingTaskList handledList;
    auto& node = m_wheel[pos];
    auto& taskList = node.taskList();
    if (taskList.empty()) continue;
    for (auto task : taskList) {
      if (task->isCancled()) {
        handledList.insert(task);
        continue;
      }

      if (0 == task->round()) {
        unique_lock<mutex> lk(m_mutexExec);
        m_expiredTaskFuncs.push_back(task->taskFunc());
        m_condExec.notify_one();
        handledList.insert(task);
        continue;
      }

      if (task->round() > 0) task->round(task->round() - 1);
    }
    for (auto& task : handledList) {
      taskList.erase(task);
    }
  }
  m_currentPos = pos;
}

NS_FF_END
