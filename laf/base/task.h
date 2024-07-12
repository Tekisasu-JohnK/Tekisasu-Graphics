// LAF Base Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_TASK_H_INCLUDED
#define BASE_TASK_H_INCLUDED
#pragma once

#include "base/debug.h"
#include "base/thread_pool.h"

#include <atomic>
#include <functional>

namespace base {

  class task;
  class thread_pool;

  class task_token {
    friend class task;
  public:
    task_token()
      : m_canceled(false)
      , m_progress(0.0f)
      , m_progress_min(0.0f)
      , m_progress_max(1.0f) { }

    bool canceled() const { return m_canceled; }
    float progress() const { return m_progress; }

    void cancel() { m_canceled = true; }
    void set_progress(float p) {
      ASSERT(p >= 0.0f && p <= 1.0f);
      m_progress = m_progress_min
        + p * (m_progress_max - m_progress_min);
    }
    void set_progress_range(float min, float max) {
      m_progress_min = min;
      m_progress_max = max;
    }

  private:
    void reset() {
      m_canceled = false;
      m_progress = 0.0f;
    }

    std::atomic<bool> m_canceled;
    std::atomic<float> m_progress;
    float m_progress_min, m_progress_max;
  };

  class task {
  public:
    typedef std::function<void(task_token&)> func_t;

    task();
    ~task();

    void on_execute(func_t&& f) { m_execute = std::move(f); }

    task_token& start(thread_pool& pool);

    bool running() const { return m_running; }

    // Returns true when the task is completed (whether it was
    // canceled or not). If this is true, it's safe to delete the task
    // instance (it will not be used anymore by any othe background
    // thread).
    bool completed() const { return m_completed; }

  private:
    void in_worker_thread();

    std::atomic<bool> m_running;
    std::atomic<bool> m_completed;
    task_token m_token;
    func_t m_execute;
  };

} // namespace base

#endif
