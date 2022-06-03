// LAF Base Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/task.h"
#include "base/thread_pool.h"

using namespace base;

TEST(Task, Basic)
{
  std::vector<task> tasks(100);
  std::atomic<int> c(0);
  thread_pool p(10);
  for (task& t : tasks) {
    t.on_execute([&c](task_token&){ ++c; });
    t.start(p);
  }
  p.wait_all();
  EXPECT_EQ(100, c);
}

TEST(Task, MultiplePools)
{
  thread_pool p1(5);
  thread_pool p2(5);

  std::vector<task> tasks1(100);
  std::vector<task> tasks2(100);

  std::atomic<int> c(0);

  for (int i=0; i<100; ++i) {
    tasks1[i].on_execute([&c](task_token&){ c += 2; });
    tasks2[i].on_execute([&c](task_token&){ --c; });
  }

  for (int i=0; i<100; ++i) {
    tasks1[i].start(p1);
    tasks2[i].start(p2);
  }

  p1.wait_all();
  p2.wait_all();
  EXPECT_EQ(100, c);

  for (int i=0; i<100; ++i)
    tasks2[i].start(p2);
  p2.wait_all();
  EXPECT_EQ(0, c);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
