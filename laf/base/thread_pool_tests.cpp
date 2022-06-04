// LAF Base Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/thread_pool.h"

#include <atomic>

using namespace base;

TEST(ThreadPool, Basic)
{
  thread_pool p(10);
  std::atomic<int> c(0);
  for (int i=0; i<10000; ++i)
    p.execute([&c]{ ++c; });
  p.wait_all();

  EXPECT_EQ(10000, c);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
