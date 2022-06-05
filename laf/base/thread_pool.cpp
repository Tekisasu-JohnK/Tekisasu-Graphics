// LAF Base Library
// Copyright (C) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/debug.h"
#include "base/log.h"
#include "base/thread_pool.h"

namespace base {

thread_pool::thread_pool(const size_t n)
  : m_running(true)
  , m_threads(n)
  , m_doingWork(0)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  for (size_t i=0; i<n; ++i)
    m_threads[i] = std::thread([this]{ worker(); });
}

thread_pool::~thread_pool()
{
  join_all();
}

void thread_pool::execute(std::function<void()>&& func)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  ASSERT(m_running);
  m_work.push(std::move(func));
  m_cv.notify_one();
}

void thread_pool::wait_all()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_cvWait.wait(lock, [this]() -> bool {
                        return
                          !m_running ||
                          (m_work.empty() && m_doingWork == 0);
                      });
}

void thread_pool::join_all()
{
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_running = false;
  }
  m_cv.notify_all();

  for (auto& j : m_threads) {
    try {
      if (j.joinable())
        j.join();
    }
    catch (const std::exception& ex) {
      LOG(FATAL, "Exception joining threads: %s\n", ex.what());
      ASSERT(false);
    }
    catch (...) {
      LOG(FATAL, "Exception joining threads\n");
      ASSERT(false);
    }
  }
}

void thread_pool::worker()
{
  bool running;
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    running = m_running;
  }
  while (running) {
    std::function<void()> func;
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() -> bool {
                        return !m_running || !m_work.empty();
                      });
      running = m_running;
      if (m_running && !m_work.empty()) {
        func = std::move(m_work.front());
        ++m_doingWork;
        m_work.pop();
      }
    }
    try {
      if (func)
        func();
    }
    // TODO handle exceptions in a better way
    catch (const std::exception& e) {
      LOG(FATAL, "Exception from worker: %s", e.what());
      ASSERT(false);
    }
    catch (...) {
      LOG(FATAL, "Exception from worker\n");
      ASSERT(false);
    }

    // Decrement m_doingWork only if we've incremented it
    if (func) {
      std::unique_lock<std::mutex> lock(m_mutex);
      --m_doingWork;
      m_cvWait.notify_all();
    }
  }
}

}
