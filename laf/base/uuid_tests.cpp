// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/uuid.h"

using namespace base;

TEST(Uuid, Empty)
{
  Uuid uuid;
  for (int i=0; i<16; ++i) {
    EXPECT_EQ(0, uuid[i]);
  }
}

TEST(Uuid, Generate)
{
  constexpr int N = 1024;
  Uuid uuids[N];

  for (int i=0; i<N; ++i)
    uuids[i] = Uuid::Generate();

  for (int i=0; i<N; ++i) {
    for (int j=0; j<N; ++j) {
      if (i == j)
        ASSERT_EQ(uuids[i], uuids[j]);
      else
        ASSERT_NE(uuids[i], uuids[j]);
    }
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
