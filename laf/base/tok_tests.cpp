// LAF Base Library
// Copyright (c) 2024 Igara Studio S.A.
// Copyright (c) 2020 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.
//
// Based on https://github.com/dacap/tok

#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "base/tok.h"

TEST(Tok, SplitTokens)
{
  int i = 0;
  auto a_result = std::vector<std::string>{ "This", "is", "a", "phrase.", "Several", "whitespaces", "are", "ignored." };
  std::string a = "This is a phrase.   Several whitespaces are ignored.";
  for (auto& tok : base::tok::split_tokens(a, ' ')) {
    std::cout << "\"" << tok << "\"\n";
    EXPECT_EQ(tok, a_result[i++]);
  }
}

TEST(Tok, Csv)
{
  int i = 0;
  auto b_result = std::vector<std::string>{ "In comma", "separated", "", "values", "", "", "empties are included" };
  std::string b = "In comma,separated,,values,,,empties are included";
  for (auto& tok : base::tok::csv(b, ',')) {
    std::cout << "\"" << tok << "\"\n";
    EXPECT_EQ(tok, b_result[i++]);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
