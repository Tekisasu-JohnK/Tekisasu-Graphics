// LAF Base Library
// Copyright (c) 2001-2018 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/fs.h"

using namespace base;

TEST(FS, MakeDirectory)
{
  EXPECT_FALSE(is_directory("a"));

  make_directory("a");
  EXPECT_TRUE(is_directory("a"));

  remove_directory("a");
  EXPECT_FALSE(is_directory("a"));
}

TEST(FS, MakeAllDirectories)
{
  EXPECT_FALSE(is_directory("a"));
  EXPECT_FALSE(is_directory("a/b"));
  EXPECT_FALSE(is_directory("a/b/c"));

  make_all_directories("a/b/c");

  EXPECT_TRUE(is_directory("a"));
  EXPECT_TRUE(is_directory("a/b"));
  EXPECT_TRUE(is_directory("a/b/c"));

  remove_directory("a/b/c");
  EXPECT_FALSE(is_directory("a/b/c"));

  remove_directory("a/b");
  EXPECT_FALSE(is_directory("a/b"));

  remove_directory("a");
  EXPECT_FALSE(is_directory("a"));
}

TEST(FS, IsPathSeparator)
{
  EXPECT_TRUE (is_path_separator('\\'));
  EXPECT_TRUE (is_path_separator('/'));
  EXPECT_FALSE(is_path_separator('a'));
  EXPECT_FALSE(is_path_separator('+'));
  EXPECT_FALSE(is_path_separator(':'));
}

TEST(FS, GetFilePath)
{
  EXPECT_EQ("C:\\foo",  get_file_path("C:\\foo\\main.cpp"));
  EXPECT_EQ("C:/foo",   get_file_path("C:/foo/pack.tar.gz"));
  EXPECT_EQ(".",        get_file_path("./main.cpp"));
  EXPECT_EQ(".",        get_file_path(".\\main.cpp"));
  EXPECT_EQ("",         get_file_path("\\main.cpp"));
  EXPECT_EQ("",         get_file_path("main.cpp"));
  EXPECT_EQ("",         get_file_path("main."));
  EXPECT_EQ("",         get_file_path("main"));
  EXPECT_EQ("C:/foo",   get_file_path("C:/foo/"));
  EXPECT_EQ("C:",       get_file_path("C:\\"));
  EXPECT_EQ("C:",       get_file_path("C:\\.cpp"));
  EXPECT_EQ("",         get_file_path(".cpp"));
  EXPECT_EQ("",         get_file_path(""));
}

TEST(FS, GetFileName)
{
  EXPECT_EQ("main.cpp",         get_file_name("C:\\foo\\main.cpp"));
  EXPECT_EQ("pack.tar.gz",      get_file_name("C:/foo/pack.tar.gz"));
  EXPECT_EQ("main.cpp",         get_file_name("./main.cpp"));
  EXPECT_EQ("main.cpp",         get_file_name(".\\main.cpp"));
  EXPECT_EQ("main.cpp",         get_file_name("\\main.cpp"));
  EXPECT_EQ("main.cpp",         get_file_name("main.cpp"));
  EXPECT_EQ("main.",            get_file_name("main."));
  EXPECT_EQ("main",             get_file_name("main"));
  EXPECT_EQ("",                 get_file_name("C:/foo/"));
  EXPECT_EQ("",                 get_file_name("C:\\"));
  EXPECT_EQ(".cpp",             get_file_name("C:\\.cpp"));
  EXPECT_EQ(".cpp",             get_file_name(".cpp"));
  EXPECT_EQ("",                 get_file_name(""));
}

TEST(FS, GetFileExtension)
{
  EXPECT_EQ("cpp",      get_file_extension("C:\\foo\\main.cpp"));
  EXPECT_EQ("gz",       get_file_extension("C:/foo/pack.tar.gz"));
  EXPECT_EQ("cpp",      get_file_extension("./main.cpp"));
  EXPECT_EQ("cpp",      get_file_extension(".\\main.cpp"));
  EXPECT_EQ("cpp",      get_file_extension("\\main.cpp"));
  EXPECT_EQ("cpp",      get_file_extension("main.cpp"));
  EXPECT_EQ("",         get_file_extension("main."));
  EXPECT_EQ("",         get_file_extension("main"));
  EXPECT_EQ("",         get_file_extension("C:/foo/"));
  EXPECT_EQ("",         get_file_extension("C:\\"));
  EXPECT_EQ("cpp",      get_file_extension("C:\\.cpp"));
  EXPECT_EQ("cpp",      get_file_extension(".cpp"));
  EXPECT_EQ("",         get_file_extension(""));
}

TEST(FS, GetFileTitle)
{
  EXPECT_EQ("main",     get_file_title("C:\\foo\\main.cpp"));
  EXPECT_EQ("pack.tar", get_file_title("C:/foo/pack.tar.gz"));
  EXPECT_EQ("main",     get_file_title("./main.cpp"));
  EXPECT_EQ("main",     get_file_title(".\\main.cpp"));
  EXPECT_EQ("main",     get_file_title("\\main.cpp"));
  EXPECT_EQ("main",     get_file_title("main.cpp"));
  EXPECT_EQ("main",     get_file_title("main."));
  EXPECT_EQ("main",     get_file_title("main"));
  EXPECT_EQ("",         get_file_title("C:/foo/"));
  EXPECT_EQ("",         get_file_title("C:\\"));
  EXPECT_EQ("",         get_file_title("C:\\.cpp"));
  EXPECT_EQ("",         get_file_title(".cpp"));
  EXPECT_EQ("",         get_file_title(""));
}

TEST(FS, GetFileTitleWithPath)
{
  EXPECT_EQ("C:\\foo\\main",   get_file_title_with_path("C:\\foo\\main.cpp"));
  EXPECT_EQ("C:/foo/pack.tar", get_file_title_with_path("C:/foo/pack.tar.gz"));
  EXPECT_EQ("./main",          get_file_title_with_path("./main.cpp"));
  EXPECT_EQ(".\\main",         get_file_title_with_path(".\\main.cpp"));
  EXPECT_EQ("\\main",          get_file_title_with_path("\\main.cpp"));
  EXPECT_EQ("main",            get_file_title_with_path("main.cpp"));
  EXPECT_EQ("main",            get_file_title_with_path("main."));
  EXPECT_EQ("main",            get_file_title_with_path("main"));
  EXPECT_EQ("C:/foo/",         get_file_title_with_path("C:/foo/"));
  EXPECT_EQ("C:\\",            get_file_title_with_path("C:\\"));
  EXPECT_EQ("C:\\",            get_file_title_with_path("C:\\.cpp"));
  EXPECT_EQ("",                get_file_title_with_path(".cpp"));
  EXPECT_EQ("",                get_file_title_with_path(""));
}

TEST(FS, JoinPath)
{
  std::string sep;
  sep.push_back(path_separator);

  EXPECT_EQ("",                         join_path("", ""));
  EXPECT_EQ("fn",                       join_path("", "fn"));
  EXPECT_EQ("/fn",                      join_path("/", "fn"));
  EXPECT_EQ("/this"+sep+"fn",           join_path("/this", "fn"));
  EXPECT_EQ("C:\\path"+sep+"fn",        join_path("C:\\path", "fn"));
  EXPECT_EQ("C:\\path\\fn",             join_path("C:\\path\\", "fn"));
}

TEST(FS, RemovePathSeparator)
{
  EXPECT_EQ("C:\\foo",                  remove_path_separator("C:\\foo\\"));
  EXPECT_EQ("C:/foo",                   remove_path_separator("C:/foo/"));
  EXPECT_EQ("C:\\foo\\main.cpp",        remove_path_separator("C:\\foo\\main.cpp"));
  EXPECT_EQ("C:\\foo\\main.cpp",        remove_path_separator("C:\\foo\\main.cpp/"));
}

TEST(FS, HasFileExtension)
{
  EXPECT_TRUE (has_file_extension("hi.png", base::paths{"png"}));
  EXPECT_FALSE(has_file_extension("hi.png", base::paths{"pngg"}));
  EXPECT_FALSE(has_file_extension("hi.png", base::paths{"ppng"}));
  EXPECT_TRUE (has_file_extension("hi.jpeg", base::paths{"jpg","jpeg"}));
  EXPECT_TRUE (has_file_extension("hi.jpg", base::paths{"jpg","jpeg"}));
  EXPECT_FALSE(has_file_extension("hi.ase", base::paths{"jpg","jpeg"}));
  EXPECT_TRUE (has_file_extension("hi.ase", base::paths{"jpg","jpeg","ase"}));
  EXPECT_TRUE (has_file_extension("hi.ase", base::paths{"ase","jpg","jpeg"}));

  EXPECT_TRUE (has_file_extension("hi.png", base::paths{"Png"}));
  EXPECT_TRUE (has_file_extension("hi.pnG", base::paths{"bmp","PNg"}));
  EXPECT_TRUE (has_file_extension("hi.bmP", base::paths{"bMP","PNg"}));
}

TEST(FS, ReplaceExtension)
{
  EXPECT_EQ("hi.bmp", replace_extension("hi.png", "bmp"));
  EXPECT_EQ("hi.bmp", replace_extension("hi", "bmp"));
  EXPECT_EQ("hi", replace_extension("hi.bmp", ""));
  EXPECT_EQ("hi", replace_extension("hi", ""));
  EXPECT_EQ(".bmp", replace_extension(".png", "bmp"));
  EXPECT_EQ("/path/hi.bmp", replace_extension("/path/hi.png", "bmp"));
  EXPECT_EQ("/path/hi.bmp", replace_extension("/path/hi", "bmp"));
  EXPECT_EQ("/path/hi", replace_extension("/path/hi.bmp", ""));
  EXPECT_EQ("/path/hi", replace_extension("/path/hi", ""));
  EXPECT_EQ("/.bmp", replace_extension("/.png", "bmp"));
}

TEST(FS, CompareFilenames)
{
  EXPECT_EQ(-1, compare_filenames("a", "b"));
  EXPECT_EQ(-1, compare_filenames("a0", "a1"));
  EXPECT_EQ(-1, compare_filenames("a0", "b1"));
  EXPECT_EQ(-1, compare_filenames("a0.png", "a1.png"));
  EXPECT_EQ(-1, compare_filenames("a1-1.png", "a1-2.png"));
  EXPECT_EQ(-1, compare_filenames("a1-2.png", "a1-10.png"));
  EXPECT_EQ(-1, compare_filenames("a1-64-2.png", "a1-64-10.png"));
  EXPECT_EQ(-1, compare_filenames("a32.txt", "a32l.txt"));
  EXPECT_EQ(-1, compare_filenames("a", "aa"));

  EXPECT_EQ(0, compare_filenames("a", "a"));
  EXPECT_EQ(0, compare_filenames("a", "A"));
  EXPECT_EQ(0, compare_filenames("a1B", "A1b"));
  EXPECT_EQ(0, compare_filenames("a32-16.txt32", "a32-16.txt32"));

  EXPECT_EQ(1, compare_filenames("aa", "a"));
  EXPECT_EQ(1, compare_filenames("b", "a"));
  EXPECT_EQ(1, compare_filenames("a1", "a0"));
  EXPECT_EQ(1, compare_filenames("b1", "a0"));
  EXPECT_EQ(1, compare_filenames("a1.png", "a0.png"));
  EXPECT_EQ(1, compare_filenames("a1-2.png", "a1-1.png"));
  EXPECT_EQ(1, compare_filenames("a1-10.png", "a1-9.png"));
  EXPECT_EQ(1, compare_filenames("a1-64-10.png", "a1-64-9.png"));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
