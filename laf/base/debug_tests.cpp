// LAF Base Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/debug.h"

TEST(Debug, ArgsToString)
{
  EXPECT_EQ("hi", base_args_to_string("hi"));
  EXPECT_EQ("hi 2 world", base_args_to_string("hi", 2, "world"));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
