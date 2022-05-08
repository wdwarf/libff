/**
 * @file Timer.cpp
 * @auth DuckyLi
 * @date 2022-05-06 11:36:41
 * @description
 */
#include <ff/Bind.h>
#include <ff/Tick.h>
#include <ff/Timer.h>

#include <cassert>
#include <functional>
#include <iostream>

using namespace std;
using namespace std::placeholders;

NS_FF_BEG

static TimerID s_id = 0;

Timer::Timer(uint32_t taskThreadCnt)
    : m_tasks(nullptr), m_threadPool(taskThreadCnt), m_stop(false) {
  this->m_timerThread = thread(&Timer::timerThreadFunc, this);
}

Timer::~Timer() {
  this->m_stop = true;
  this->m_cond.notify_one();
  if (this->m_timerThread.joinable()) {
    this->m_timerThread.join();
  }

  auto p = this->m_tasks;
  while (p) {
    auto tmp = p;
    p = p->next;
    delete tmp;
  }

  for (auto& p : this->m_insertTasks) {
    delete p;
  }
}

int64_t Timer::getNextWaitMs() {
  if (nullptr != m_tasks) {
    auto tock = m_tick.tock();
    return (this->m_tasks->deadline > tock ? (this->m_tasks->deadline - tock)
                                           : 0);
  }
  return -1;
}

void Timer::timerThreadFunc() {
  Tick t;
  uint32_t n = 0;
  while (!this->m_stop) {
    processNewTasks();
    {
      unique_lock<mutex> lk(this->m_mutex);

      auto mil = this->getNextWaitMs();
      (mil >= 0) ? ((void)this->m_cond.wait_for(lk, chrono::milliseconds(mil)))
                 : this->m_cond.wait(lk);

      if (nullptr == this->m_tasks) continue;
      if (this->m_stop) break;

      auto p = this->m_tasks;

      while (p) {
        auto task = p;
        p = p->next;

        if (this->m_tick.tock() >= task->deadline || task->cancled) {
          auto prev = task->prev;
          auto next = task->next;

          m_threadPool.exec(bind(&Timer::execTask, this, task));

          if (nullptr == prev) {
            this->m_tasks = next;
            if (nullptr != this->m_tasks) this->m_tasks->prev = nullptr;
            continue;
          }

          prev->next = next;
          next->prev = prev;
        }
      }
    }
  }
}

void Timer::execTask(PTimerTask task) {
  if (!task->cancled) {
    task->func();
    if (TimerTaskType::Repeat == task->type) {
      this->insertTask(task);
      return;
    }
  }

  delete task;
}

void Timer::insertTask(PTimerTask task) {
  lock_guard<mutex> lk(this->m_mutexNewTask);
  if (this->m_stop) {
    delete task;
    return;
  }
  task->deadline = m_tick.tock() + task->timeout;
  m_insertTasks.push_back(task);
  this->m_cond.notify_one();
}

void Timer::processNewTasks() {
  unique_lock<mutex> lk(this->m_mutex);
  lock_guard<mutex> lk2(this->m_mutexNewTask);
  if (this->m_insertTasks.empty()) return;

  for (auto& task : this->m_insertTasks) {
    this->processNewTask(task);
  }
  this->m_insertTasks.clear();
}

void Timer::processNewTask(PTimerTask task) {
  if (nullptr == this->m_tasks) {
    this->m_tasks = task;
    this->m_tasks->next = nullptr;
    this->m_tasks->prev = nullptr;
    return;
  }

  auto p = this->m_tasks;
  while (true) {
    if (p->deadline >= task->deadline) {
      auto prev = p->prev;
      if (nullptr == prev) {
        task->next = p;
        task->prev = nullptr;
        p->prev = task;
        this->m_tasks = task;
        return;
      }

      prev->next = task;
      task->prev = prev;

      task->next = p;
      p->prev = task;
      return;
    }

    if (nullptr == p->next) break;
    p = p->next;
  }
  p->next = task;
  task->prev = p;
  task->next = nullptr;
}

TimerID Timer::setTimeout(TimerFunc func, uint64_t timeoutMs) {
  auto task = new TimerTask;
  {
    lock_guard<mutex> lk(this->m_mutexNewTask);
    task->func = func;
    task->id = s_id++;
    task->timeout = timeoutMs;
    task->type = TimerTaskType::Once;
  }
  this->insertTask(task);
  return task->id;
}

TimerID Timer::setInterval(TimerFunc func, uint64_t intervalMs) {
  auto task = new TimerTask;
  {
    lock_guard<mutex> lk(this->m_mutexNewTask);
    task->func = func;
    task->id = s_id++;
    task->timeout = intervalMs;
    task->type = TimerTaskType::Repeat;
  }
  this->m_reapetTasks.push_back(task);
  this->insertTask(task);
  return task->id;
}

void Timer::cancelTimeout(TimerID id) {
  if (this_thread::get_id() != this->m_timerThread.get_id()) {
    unique_lock<mutex> lk(this->m_mutex);
  }

  lock_guard<mutex> lk2(this->m_mutexNewTask);

  auto p = this->m_tasks;
  while (p) {
    if (p->id == id) {
      p->cancled = true;
      this->m_cond.notify_one();
      return;
    }
    p = p->next;
  }

  for (auto& t : this->m_insertTasks) {
    if (t->id == id) {
      t->cancled = true;
      this->m_cond.notify_one();
      return;
    }
  }

  for (auto it = this->m_reapetTasks.begin(); it != this->m_reapetTasks.end();
       ++it) {
    auto t = *it;
    if (t->id == id) {
      this->m_reapetTasks.erase(it);
      t->cancled = true;
      this->m_cond.notify_one();
      return;
    }
  }
}

NS_FF_END
