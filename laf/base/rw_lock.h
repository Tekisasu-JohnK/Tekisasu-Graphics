// LAF Base Library
// Copyright (C) 2020-2023  Igara Studio S.A.
// Copyright (C) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_RW_LOCK_H_INCLUDED
#define BASE_RW_LOCK_H_INCLUDED
#pragma once

#include "base/disable_copying.h"

#include <atomic>
#include <mutex>
#include <thread>

namespace base {

  // A readers-writer lock implementation
  class RWLock {
  public:
    enum LockType {
      ReadLock,
      WriteLock
    };

    enum WeakLock {
      WeakUnlocked,
      WeakUnlocking,
      WeakLocked,
    };

    // Result of a lock operation. This result must be used in the
    // unlock function to know if we should really unlock it
    // (e.g. only the first write-lock in the same thread will
    // write-unlock it).
    enum class LockResult {
      Fail,      // Cannot lock now
      Reentrant, // Already locked in thread (we can continue)
      OK,        // Locked right now in this thread
    };

    RWLock();
    ~RWLock();

    // Returns true if we can lock this object for writing purposes in
    // case that the current thread has it locked for reading.
    bool canWriteLockFromRead() const;

    // Locks the object to read or write on it, returning OK if the
    // object can be accessed in the desired mode, ReentrantLock if
    // the mode is compatible with the thread that locked this object,
    // or Failed if the mode is incompatible.
    LockResult lock(LockType lockType, int timeout);

    // If you've locked the object to read, using this method you can
    // raise your access level to write it.
    LockResult upgradeToWrite(int timeout);

    // If we've locked the object to write, using this method we can
    // lower our access to read-only.
    void downgradeToRead(LockResult lockResult);

    // Unlocks a previously successfully lock() operation.
    void unlock(LockResult lockResult);

    // Tries to lock the object for read access in a "weak way" so
    // other thread (e.g. UI thread) can lock the object removing this
    // weak lock.
    //
    // The "weak_lock_flag" is used to notify when the "weak lock" is
    // lost.
    bool weakLock(std::atomic<WeakLock>* weak_lock_flag);
    void weakUnlock();

  private:
    // Mutex to modify the 'locked' flag.
    mutable std::mutex m_mutex;

    // True if some thread is writing the object.
    bool m_write_lock = false;
    std::thread::id m_write_thread = {};

    // Greater than zero when one or more threads are reading the object.
    int m_read_locks = 0;

    // If this isn' nullptr, it means that it points to an unique
    // "weak" lock that can be unlocked from other thread. E.g. the
    // backup/data recovery thread might weakly lock the object so if
    // the user UI thread needs the object again, the backup process
    // can stop.
    std::atomic<WeakLock>* m_weak_lock = nullptr;

    DISABLE_COPYING(RWLock);
  };

} // namespace base

#endif
