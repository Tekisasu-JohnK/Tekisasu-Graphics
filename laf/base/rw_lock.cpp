// LAF Base Library
// Copyright (C) 2020-2024  Igara Studio S.A.
// Copyright (C) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/rw_lock.h"

// Uncomment this line in case that you want TRACE() lock/unlock
// operations.
//#define DEBUG_OBJECT_LOCKS

#include "base/debug.h"
#include "base/thread.h"

#include <algorithm>

namespace base {

RWLock::RWLock()
{
}

RWLock::~RWLock()
{
  ASSERT(!m_write_lock);
  ASSERT(m_write_thread == std::thread::id());
  ASSERT(m_read_locks == 0);
  ASSERT(m_weak_lock == nullptr);
}

bool RWLock::canWriteLockFromRead() const
{
  const std::lock_guard lock(m_mutex);

  // If this thread already have a writer lock, we can upgrade any
  // reader lock to writer in the same thread (re-entrant locks).
  if (m_write_lock &&
      m_write_thread == std::this_thread::get_id()) {
    return true;
  }
  // If only we are reading (one lock) and nobody is writing, we can
  // lock for writing..
  return (m_read_locks == 1 && !m_write_lock);
}

RWLock::LockResult RWLock::lock(LockType lockType, int timeout)
{
  // Check for re-entrant write locks (multiple write-lock in the same
  // thread are allowed).
  // if (lockType == WriteLock) {
  {
    const std::lock_guard lock(m_mutex);
    if (m_write_lock &&
        m_write_thread == std::this_thread::get_id()) {
      return LockResult::Reentrant;
    }
  }

  while (timeout >= 0) {
    {
      const std::lock_guard lock(m_mutex);

      switch (lockType) {

        case ReadLock:
          // If no body is writing the object...
          if (!m_write_lock) {
            // We can read it
            ++m_read_locks;
            return LockResult::OK;
          }
          break;

        case WriteLock:
          // Check that there is no weak lock
          if (m_weak_lock) {
            if (*m_weak_lock == WeakLocked)
              *m_weak_lock = WeakUnlocking;

            if (*m_weak_lock == WeakUnlocking)
              goto go_wait;

            ASSERT(*m_weak_lock == WeakUnlocked);
          }

          // If no body is reading and writing...
          if (m_read_locks == 0 && !m_write_lock) {
            // We can start writing the object...
            m_write_lock = true;
            m_write_thread = std::this_thread::get_id();

#ifdef DEBUG_OBJECT_LOCKS
            TRACE("LCK: lock: Locked <%p> to write\n", this);
#endif
            return LockResult::OK;
          }
          break;

      }

    go_wait:;
    }

    if (timeout > 0) {
      const int delay = std::min(100, timeout);
      timeout -= delay;

#ifdef DEBUG_OBJECT_LOCKS
      TRACE("LCK: lock: wait 100 msecs for <%p>\n", this);
#endif

      base::this_thread::sleep_for(double(delay) / 1000.0);
    }
    else
      break;
  }

#ifdef DEBUG_OBJECT_LOCKS
  TRACE("LCK: lock: Cannot lock <%p> to %s (has %d read locks and %d write locks)\n",
    this, (lockType == ReadLock ? "read": "write"), m_read_locks, m_write_lock);
#endif

  return LockResult::Fail;
}

void RWLock::downgradeToRead(LockResult lockResult)
{
  if (lockResult != LockResult::OK)
    return; // Do nothing for failed or reentrant locks

  const std::lock_guard lock(m_mutex);

  ASSERT(m_read_locks == 0);
  ASSERT(m_write_lock);

  m_write_lock = false;
  m_write_thread = std::thread::id();
  m_read_locks = 1;
}

void RWLock::unlock(LockResult lockResult)
{
  if (lockResult != LockResult::OK)
    return; // Do nothing for failed or reentrant locks

  const std::lock_guard lock(m_mutex);

  if (m_write_lock) {
    m_write_lock = false;
    m_write_thread = std::thread::id();
  }
  else if (m_read_locks > 0) {
    --m_read_locks;
  }
  else {
    ASSERT(false);
  }
}

bool RWLock::weakLock(std::atomic<WeakLock>* weak_lock_flag)
{
  const std::lock_guard lock(m_mutex);

  if (m_weak_lock ||
      m_write_lock)
    return false;

  m_weak_lock = weak_lock_flag;
  *m_weak_lock = WeakLocked;
  return true;
}

void RWLock::weakUnlock()
{
  const std::lock_guard lock(m_mutex);

  ASSERT(m_weak_lock);
  ASSERT(*m_weak_lock != WeakLock::WeakUnlocked);
  ASSERT(!m_write_lock);

  if (m_weak_lock) {
    *m_weak_lock = WeakLock::WeakUnlocked;
    m_weak_lock = nullptr;
  }
}

RWLock::LockResult RWLock::upgradeToWrite(int timeout)
{
  // Check for re-entrant upgrade to write (multiple write-lock in the
  // same thread are allowed).
  {
    const std::lock_guard lock(m_mutex);
    if (m_write_lock &&
        m_write_thread == std::this_thread::get_id()) {
      return LockResult::Reentrant;
    }
  }

  while (timeout >= 0) {
    {
      const std::lock_guard lock(m_mutex);

      // Check that there is no weak lock
      if (m_weak_lock) {
        if (*m_weak_lock == WeakLocked)
          *m_weak_lock = WeakUnlocking;

        // Wait some time
        if (*m_weak_lock == WeakUnlocking)
          goto go_wait;

        ASSERT(*m_weak_lock == WeakUnlocked);
      }

      // this only is possible if there are just one reader
      if (m_read_locks == 1) {
        ASSERT(!m_write_lock);
        m_read_locks = 0;
        m_write_lock = true;
        m_write_thread = std::this_thread::get_id();

#ifdef DEBUG_OBJECT_LOCKS
        TRACE("LCK: upgradeToWrite: Locked <%p> to write\n", this);
#endif

        return LockResult::OK;
      }

    go_wait:;
    }

    if (timeout > 0) {
      const int delay = std::min(100, timeout);
      timeout -= delay;

#ifdef DEBUG_OBJECT_LOCKS
      TRACE("LCK: upgradeToWrite: wait 100 msecs for <%p>\n", this);
#endif

      base::this_thread::sleep_for(double(delay) / 1000.0);
    }
    else
      break;
  }

#ifdef DEBUG_OBJECT_LOCKS
  TRACE("LCK: upgradeToWrite: Cannot lock <%p> to write (has %d read locks and %d write locks)\n",
    this, m_read_locks, m_write_lock);
#endif

  return LockResult::Fail;
}

} // namespace base
