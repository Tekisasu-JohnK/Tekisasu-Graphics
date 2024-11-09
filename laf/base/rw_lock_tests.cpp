// LAF Base Library
// Copyright (c) 2020 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/rw_lock.h"

#include <thread>

using namespace base;
using LockResult = RWLock::LockResult;

#define EXPECT_FAIL(a)      EXPECT_EQ(LockResult::Fail, a)
#define EXPECT_REENTRANT(a) EXPECT_EQ(LockResult::Reentrant, a)
#define EXPECT_OK(a)        EXPECT_EQ(LockResult::OK, a)

#define BGTHREAD(code)      std::thread([&a]{ code; }).join();

TEST(RWLock, MultipleReaders)
{
  RWLock a;
  LockResult res[5];
  EXPECT_OK(res[0] = a.lock(RWLock::ReadLock, 0));
  EXPECT_OK(res[1] = a.lock(RWLock::ReadLock, 0));
  EXPECT_OK(res[2] = a.lock(RWLock::ReadLock, 0));
  EXPECT_OK(res[3] = a.lock(RWLock::ReadLock, 0));
  EXPECT_FAIL(res[4] = a.lock(RWLock::WriteLock, 0));
  a.unlock(res[3]);
  a.unlock(res[2]);
  a.unlock(res[1]);
  a.unlock(res[0]);
}

TEST(RWLock, OneWriter)
{
  RWLock a;
  LockResult res[2];

  EXPECT_OK(res[0] = a.lock(RWLock::WriteLock, 0));
  BGTHREAD(EXPECT_FAIL(a.lock(RWLock::ReadLock, 0)));
  EXPECT_REENTRANT(res[1] = a.lock(RWLock::ReadLock, 0));
  a.unlock(res[1]);
  a.unlock(res[0]);

  EXPECT_OK(res[0] = a.lock(RWLock::ReadLock, 0));
  EXPECT_FAIL(a.lock(RWLock::WriteLock, 0));
  a.unlock(res[0]);

  EXPECT_OK(res[0] = a.lock(RWLock::ReadLock, 0));
  EXPECT_OK(res[1] = a.lock(RWLock::ReadLock, 0));
  EXPECT_FAIL(a.lock(RWLock::WriteLock, 0));
  a.unlock(res[1]);
  EXPECT_FAIL(a.lock(RWLock::WriteLock, 0));
  a.unlock(res[0]);
  EXPECT_OK(res[0] = a.lock(RWLock::WriteLock, 0));
  EXPECT_REENTRANT(res[1] = a.lock(RWLock::WriteLock, 0));
  BGTHREAD(EXPECT_FAIL(a.lock(RWLock::WriteLock, 0)));
  a.unlock(res[1]);
  a.unlock(res[0]);
}

TEST(RWLock, UpgradeToWrite)
{
  RWLock a;
  LockResult res[3];

  EXPECT_OK(res[0] = a.lock(RWLock::ReadLock, 0));
  EXPECT_FAIL(a.lock(RWLock::WriteLock, 0));
  EXPECT_OK(res[1] = a.upgradeToWrite(0));
  BGTHREAD(EXPECT_FAIL(a.lock(RWLock::ReadLock, 0)));
  EXPECT_REENTRANT(res[2] = a.lock(RWLock::WriteLock, 0));
  a.unlock(res[2]);
  a.downgradeToRead(res[1]);
  a.unlock(res[0]);
}

TEST(RWLock, WeakLock)
{
  RWLock a;
  LockResult res[1];
  std::atomic<RWLock::WeakLock> flag(RWLock::WeakUnlocked);

  EXPECT_TRUE(a.weakLock(&flag));
  EXPECT_EQ(RWLock::WeakLocked, flag);

  // We can lock for read-only without timeout and the weak lock
  // should stay locked.
  EXPECT_OK(res[0] = a.lock(RWLock::ReadLock, 0));
  a.unlock(res[0]);             // Unlock the read-only lock
  EXPECT_EQ(RWLock::WeakLocked, flag);

  // If we want to lock for writing purposes, it should fail, but the
  // weak lock should be released.
  EXPECT_FAIL(a.lock(RWLock::WriteLock, 0));
  EXPECT_EQ(RWLock::WeakUnlocking, flag);
  a.weakUnlock();
  EXPECT_EQ(RWLock::WeakUnlocked, flag);

  EXPECT_OK(res[0] = a.lock(RWLock::WriteLock, 0));
  EXPECT_FALSE(a.weakLock(&flag));
  a.unlock(res[0]);             // Unlock the write lock
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
