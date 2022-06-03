// LAF Base Library
// Copyright (C) 2018  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/file_content.h"

using namespace base;

TEST(FileContent, ReadWrite)
{
  const char* fn = "_test_.tmp";

  for (size_t s : { 30, 500, 1024*64, 1024*64*3+4 }) {
    buffer buf(s);
    for (int i=0; i<buf.size(); ++i)
      buf[i] = i;

    write_file_content(fn, buf);
    buffer buf2 = read_file_content(fn);

    EXPECT_EQ(buf, buf2);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
