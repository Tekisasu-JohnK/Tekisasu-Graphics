// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/scoped_value.h"

using namespace base;

TEST(ScopedValue, Bool)
{
  bool a = false;
  EXPECT_FALSE(a);
  {
    ScopedValue scoped(a, true);
    EXPECT_TRUE(a);
  }
  EXPECT_FALSE(a);
}

TEST(ScopedValue, Int)
{
  int a = 1;
  EXPECT_EQ(1, a);
  {
    ScopedValue scoped1(a, 2, 4);
    EXPECT_EQ(2, a);
    {
      ScopedValue scoped2(a, 3);
      EXPECT_EQ(3, a);
    }
    EXPECT_EQ(2, a);
  }
  EXPECT_EQ(4, a);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
