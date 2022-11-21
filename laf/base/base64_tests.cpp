// LAF Base Library
// Copyright (c) 2022 Igara Studio S.A.
// Copyright (c) 2015-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/base64.h"
#include "base/string.h"

using namespace base;

TEST(Base64, Encode)
{
  EXPECT_EQ("", encode_base64(buffer()));
  EXPECT_EQ("Cg==", encode_base64(buffer{'\n'}));
  EXPECT_EQ("YQ==", encode_base64(buffer{'a'}));
  EXPECT_EQ("YWJjZGU=", encode_base64(buffer{'a', 'b', 'c', 'd', 'e'}));
  EXPECT_EQ("YWJjZGU=", encode_base64("abcde"));
  EXPECT_EQ("YWJj", encode_base64("abc"));
  EXPECT_EQ("5pel5pys6Kqe", encode_base64("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E")); // "日本語"
}

TEST(Base64, Decode)
{
  EXPECT_EQ(buffer(), decode_base64(""));
  EXPECT_EQ(buffer{'\n'}, decode_base64("Cg=="));
  EXPECT_EQ(buffer{'a'}, decode_base64("YQ=="));
  EXPECT_EQ(buffer({'a', 'b', 'c', 'd', 'e'}), decode_base64("YWJjZGU="));
  EXPECT_EQ("abcde", decode_base64s("YWJjZGU="));
  EXPECT_EQ("abc", decode_base64s("YWJj"));
  EXPECT_EQ("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E", decode_base64s("5pel5pys6Kqe")); // "日本語"
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
