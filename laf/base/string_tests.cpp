// LAF Base Library
// Copyright (c) 2022 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/string.h"
#include "base/utf8_decode.h"

#include <algorithm>
#include <clocale>

using namespace base;

int count_utf8_codepoints(const std::string& str) {
  int count = 0;
  utf8_decode dec(str);
  while (dec.next())
    ++count;
  return count;
}

TEST(String, Utf8Conversion)
{
  std::string a = "\xE6\xBC\xA2\xE5\xAD\x97"; // 漢字
  ASSERT_EQ(6, a.size());

  std::wstring b = from_utf8(a);
  ASSERT_EQ(2, b.size());
  ASSERT_EQ(0x6f22, b[0]);
  ASSERT_EQ(0x5b57, b[1]);

  std::string c = to_utf8(b);
  ASSERT_EQ(a, c);
}

TEST(String, Utf8Decode)
{
  std::string a, b = "abc";
  utf8_decode dec(b);
  while (const int ch = dec.next())
    a.push_back(ch);
  EXPECT_EQ("abc", a);

  std::string c, d = "def";
  dec = utf8_decode(d);
  while (const int ch = dec.next())
    c.push_back(ch);
  EXPECT_EQ("def", c);

  int i = 0;
  d = "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E";
  dec = utf8_decode(d);
  while (const int ch = dec.next()) { // 日本語
    switch (i++) {
      case 0: EXPECT_EQ(ch, 0x65E5); break;
      case 1: EXPECT_EQ(ch, 0x672C); break;
      case 2: EXPECT_EQ(ch, 0x8A9E); break;
      default: EXPECT_FALSE(true); break;
    }
  }

  std::string e = "Hello";
  utf8_decode e_dec(e);
  ASSERT_EQ(5, count_utf8_codepoints(e));
  ASSERT_EQ('H', e_dec.next());
  ASSERT_EQ('e', e_dec.next());
  ASSERT_EQ('l', e_dec.next());
  ASSERT_EQ('l', e_dec.next());
  ASSERT_EQ('o', e_dec.next());
  ASSERT_EQ(0, e_dec.next());

  std::string f = "Copyright \xC2\xA9";
  utf8_decode f_dec(f);
  ASSERT_EQ(11, count_utf8_codepoints(f));
  ASSERT_EQ('C', f_dec.next());
  ASSERT_EQ('o', f_dec.next());
  for (int i=0; i<8; ++i)       // Skip 8 chars
    f_dec.next();
  ASSERT_EQ(0xA9, f_dec.next());
  ASSERT_EQ(0, f_dec.next());

  std::string g = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
  utf8_decode g_dec(g);
  ASSERT_EQ(3, count_utf8_codepoints(g));
  ASSERT_EQ(0x10346, g_dec.next());
  ASSERT_EQ(0x65E5, g_dec.next());
  ASSERT_EQ(0x448, g_dec.next());
  ASSERT_EQ(0, g_dec.next());

  std::string h = "\xf0\xa4\xad\xa2";
  utf8_decode h_dec(h);
  ASSERT_EQ(1, count_utf8_codepoints(h));
  ASSERT_EQ(0x24B62, h_dec.next());
  ASSERT_EQ(0, h_dec.next());
}

TEST(String, Utf8ICmp)
{
  EXPECT_EQ(-1, utf8_icmp("a", "b"));
  EXPECT_EQ(-1, utf8_icmp("a", "b", 1));
  EXPECT_EQ(-1, utf8_icmp("a", "b", 2));
  EXPECT_EQ(-1, utf8_icmp("a", "aa"));
  EXPECT_EQ(-1, utf8_icmp("A", "aa", 3));
  EXPECT_EQ(-1, utf8_icmp("a", "ab"));

  EXPECT_EQ(0, utf8_icmp("AaE", "aae"));
  EXPECT_EQ(0, utf8_icmp("AaE", "aae", 3));
  EXPECT_EQ(0, utf8_icmp("a", "aa", 1));
  EXPECT_EQ(0, utf8_icmp("a", "ab", 1));

  EXPECT_EQ(1, utf8_icmp("aaa", "Aa", 3));
  EXPECT_EQ(1, utf8_icmp("Bb", "b"));
  EXPECT_EQ(1, utf8_icmp("z", "b"));
  EXPECT_EQ(1, utf8_icmp("z", "b", 1));
  EXPECT_EQ(1, utf8_icmp("z", "b", 2));
}

TEST(String, StringToLowerByUnicodeCharIssue1065)
{
  // Required to make old string_to_lower() version fail.
  std::setlocale(LC_ALL, "en-US");

  std::string  a = "\xC2\xBA";
  std::wstring b = from_utf8(a);
  std::string  c = to_utf8(b);

  ASSERT_EQ(a, c);
  ASSERT_EQ("\xC2\xBA", c);

  ASSERT_EQ(1, utf8_length(a));
  ASSERT_EQ(1, b.size());
  ASSERT_EQ(1, utf8_length(c));

  std::string d = string_to_lower(c);
  ASSERT_EQ(a, d);
  ASSERT_EQ(c, d);
  ASSERT_EQ(1, utf8_length(d));

  utf8_decode d_dec(d);
  int i = 0;
  while (const int chr = d_dec.next()) {
    ASSERT_EQ(b[i++], chr);
  }
}

// Decoding invalid utf-8 strings shouldn't crash (just mark the
// decoding state as invalid).
TEST(String, Utf8DecodeDontCrash)
{
  auto decodeAllChars =
    [](const std::string& str, bool shouldBeValid) -> int {
      utf8_decode decode(str);
      int chrs = 0;
      while (int chr = decode.next()) {
        ++chrs;
      }
      if (shouldBeValid)
        EXPECT_TRUE(decode.is_valid());
      else
        EXPECT_FALSE(decode.is_valid());
      return chrs;
    };

  std::string str = "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E"; // 日本語
  ASSERT_EQ(9, str.size());
  bool valid_decoding[10] = { true, // Empty string is decoded correctly
                              false, false, true,
                              false, false, true,
                              false, false, true };
  int decoded_chars[10] = { 0,
                            0, 0, 1,
                            1, 1, 2,
                            2, 2, 3 };

  for (int n=0; n<=str.size(); ++n) {
    int chrs = decodeAllChars(str.substr(0, n).c_str(),
                              valid_decoding[n]);
    EXPECT_EQ(decoded_chars[n], chrs);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
