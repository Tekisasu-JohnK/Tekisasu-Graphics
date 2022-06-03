// LAF Base Library
// Copyright (c) 2019  Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_CONCURRENT_QUEUE_H_INCLUDED
#define BASE_CONCURRENT_QUEUE_H_INCLUDED
#pragma once

#include "base/disable_copying.h"
#include "base/mutex.h"
#include "base/scoped_lock.h"

#include <algorithm>
#include <deque>

namespace base {

  template<typename T>
  class concurrent_queue {
  public:
    concurrent_queue() {
    }

    ~concurrent_queue() {
    }

    bool empty() const {
      bool result;
      {
        scoped_lock hold(m_mutex);
        result = m_queue.empty();
      }
      return result;
    }

    void push(const T& value) {
      scoped_lock hold(m_mutex);
      m_queue.push_back(value);
    }

    bool try_pop(T& value) {
      if (!m_mutex.try_lock())
        return false;

      scoped_unlock unlock(m_mutex);
      if (m_queue.empty())
        return false;

      value = m_queue.front();
      m_queue.pop_front();
      return true;
    }

    template<typename UnaryPredicate>
    void prioritize(UnaryPredicate p) {
      scoped_lock hold(m_mutex);

      auto it = std::find_if(m_queue.begin(), m_queue.end(), p);
      if (it != m_queue.end()) {
        T value(std::move(*it));
        m_queue.erase(it);
        m_queue.push_front(std::move(value));
      }
    }

  private:
    std::deque<T> m_queue;
    mutable mutex m_mutex;

    DISABLE_COPYING(concurrent_queue);
  };

} // namespace base

#endif
