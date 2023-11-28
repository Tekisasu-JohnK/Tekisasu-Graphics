// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/memory.h"

TEST(Memory, AlignedAlloc)
{
  void* a = base_aligned_alloc(9, 8);
  void* b = base_aligned_alloc(1, 16);
  void* c = base_aligned_alloc(16, 16);
  void* d = base_aligned_alloc(17, 16);
  void* e = base_aligned_alloc(33, 32);
  EXPECT_EQ(0, (((size_t)a) % 8));
  EXPECT_EQ(0, (((size_t)b) % 16));
  EXPECT_EQ(0, (((size_t)c) % 16));
  EXPECT_EQ(0, (((size_t)d) % 16));
  EXPECT_EQ(0, (((size_t)e) % 32));
  base_aligned_free(a);
  base_aligned_free(b);
  base_aligned_free(c);
  base_aligned_free(d);
  base_aligned_free(e);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
