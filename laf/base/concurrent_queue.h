// LAF Base Library
// Copyright (c) 2019-2022 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_CONCURRENT_QUEUE_H_INCLUDED
#define BASE_CONCURRENT_QUEUE_H_INCLUDED
#pragma once

#include <algorithm>
#include <deque>
#include <mutex>

namespace base {

  template<typename T>
  class concurrent_queue {
  public:
    concurrent_queue() { }
    concurrent_queue(const concurrent_queue&) = delete;
    concurrent_queue& operator=(const concurrent_queue&) = delete;
    ~concurrent_queue() { }

    bool empty() const {
      bool result;
      {
        std::lock_guard lock(m_mutex);
        result = m_queue.empty();
      }
      return result;
    }

    void clear() {
      std::lock_guard lock(m_mutex);
      m_queue.clear();
    }

    size_t size() const {
      size_t result;
      {
        std::lock_guard lock(m_mutex);
        result = m_queue.size();
      }
      return result;
    }

    void push(const T& value) {
      std::lock_guard lock(m_mutex);
      m_queue.push_back(value);
    }

    bool try_pop(T& value) {
      if (!m_mutex.try_lock())
        return false;

      std::lock_guard unlock(m_mutex, std::adopt_lock);
      if (m_queue.empty())
        return false;

      value = m_queue.front();
      m_queue.pop_front();
      return true;
    }

    template<typename UnaryPredicate>
    void prioritize(UnaryPredicate p) {
      std::lock_guard lock(m_mutex);

      auto it = std::find_if(m_queue.begin(), m_queue.end(), p);
      if (it != m_queue.end()) {
        T value(std::move(*it));
        m_queue.erase(it);
        m_queue.push_front(std::move(value));
      }
    }

  private:
    std::deque<T> m_queue;
    mutable std::mutex m_mutex;
  };

} // namespace base

#endif
