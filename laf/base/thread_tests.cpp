// LAF Base Library
// Copyright (c) 2023 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/thread.h"

using namespace base;

TEST(Thread, SetGetName)
{
  // By default the thread name can be empty (Windows, macOS) or the
  // process name (Linux).
#if LAF_LINUX
  EXPECT_EQ("thread_tests", this_thread::get_name());
#else
  EXPECT_EQ("", this_thread::get_name());
#endif

  this_thread::set_name("main");
  EXPECT_EQ("main", this_thread::get_name());

  this_thread::set_name("testing");
  EXPECT_EQ("testing", this_thread::get_name());
}

TEST(Thread, NameLimits)
{
  const char* fullName = "123456789012345678901234567890"
                         "123456789012345678901234567890"
                         "123456789012345678901234567890";

  this_thread::set_name(fullName);

#if LAF_WINDOWS
  EXPECT_EQ(fullName, this_thread::get_name());
#elif LAF_MACOS
  // Limited to 64 chars (including the null char)
  EXPECT_EQ("123456789012345678901234567890"
            "123456789012345678901234567890"
            "123", this_thread::get_name());
#else
  // Limited to 16 chars (including the null char)
  EXPECT_EQ("123456789012345", this_thread::get_name());
#endif
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
