// LAF Base Library
// Copyright (C) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_THREAD_POOL_H_INCLUDED
#define BASE_THREAD_POOL_H_INCLUDED
#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace base {

  class thread_pool {
  public:
    thread_pool(const size_t n);
    ~thread_pool();

    void execute(std::function<void()>&& func);

    // Waits until the queue is empty.
    void wait_all();

  private:
    // Joins all threads without waiting the queue to be processed.
    void join_all();

    // Called for each worker thread.
    void worker();

    bool m_running;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::condition_variable m_cvWait;
    std::queue<std::function<void()>> m_work;
    int m_doingWork;
  };

}

#endif
