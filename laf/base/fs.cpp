// LAF Base Library
// Copyright (c) 2001-2018 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/fs.h"
#include "base/split_string.h"
#include "base/string.h"

#ifdef _WIN32
  #include "base/fs_win32.h"
#else
  #include "base/fs_unix.h"
#endif

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iterator>

namespace base {

#ifdef _WIN32
  const std::string::value_type path_separator = '\\';
#else
  const std::string::value_type path_separator = '/';
#endif

void make_all_directories(const std::string& path)
{
  std::vector<std::string> parts;
  split_string(path, parts, "/\\");

  std::string intermediate;
  for (const std::string& component : parts) {
    if (component.empty()) {
      if (intermediate.empty())
        intermediate += "/";
      continue;
    }

    intermediate = join_path(intermediate, component);

    if (is_file(intermediate))
      throw std::runtime_error("Error creating directory (a component is a file name)");
    else if (!is_directory(intermediate))
      make_directory(intermediate);
  }
}

std::string get_absolute_path(const std::string& filename)
{
  std::string fn = filename;
  if (fn.size() > 2 &&
#ifdef _WIN32
      fn[1] != ':'
#else
      fn[0] != '/'
#endif
      ) {
    fn = base::join_path(base::get_current_path(), fn);
  }
  fn = base::get_canonical_path(fn);
  return fn;
}

bool is_path_separator(std::string::value_type chr)
{
  return (chr == '\\' || chr == '/');
}

std::string get_file_path(const std::string& filename)
{
  std::string::const_reverse_iterator rit;
  std::string res;

  for (rit=filename.rbegin(); rit!=filename.rend(); ++rit)
    if (is_path_separator(*rit))
      break;

  if (rit != filename.rend()) {
    ++rit;
    std::copy(filename.begin(), std::string::const_iterator(rit.base()),
              std::back_inserter(res));
  }

  return res;
}

std::string get_file_name(const std::string& filename)
{
  std::string::const_reverse_iterator rit;
  std::string result;

  for (rit=filename.rbegin(); rit!=filename.rend(); ++rit)
    if (is_path_separator(*rit))
      break;

  std::copy(std::string::const_iterator(rit.base()), filename.end(),
            std::back_inserter(result));

  return result;
}

std::string get_file_extension(const std::string& filename)
{
  std::string::const_reverse_iterator rit;
  std::string result;

  // search for the first dot from the end of the string
  for (rit=filename.rbegin(); rit!=filename.rend(); ++rit) {
    if (is_path_separator(*rit))
      return result;
    else if (*rit == '.')
      break;
  }

  if (rit != filename.rend()) {
    std::copy(std::string::const_iterator(rit.base()), filename.end(),
              std::back_inserter(result));
  }

  return result;
}

std::string replace_extension(const std::string& filename, const std::string& extension)
{
  std::string::const_reverse_iterator rit;
  std::string result;

  // Search for the first dot from the end of the string.
  for (rit=filename.rbegin(); rit!=filename.rend(); ++rit) {
    // Here is the dot of the extension.
    if (*rit == '.')
      break;
    // A path separator before a dot, i.e. the filename doesn't have a
    // extension.
    else if (is_path_separator(*rit)) {
      rit = filename.rend();
      break;
    }
  }

  if (rit != filename.rend()) {
    auto it = std::string::const_iterator(rit.base());
    --it;
    std::copy(filename.begin(), it,
              std::back_inserter(result));
  }
  else {
    result = filename;
  }

  if (!extension.empty()) {
    result.push_back('.');
    result += extension;
  }

  return result;
}


std::string get_file_title(const std::string& filename)
{
  std::string::const_reverse_iterator rit;
  std::string::const_iterator last_dot = filename.end();
  std::string result;

  for (rit=filename.rbegin(); rit!=filename.rend(); ++rit) {
    if (is_path_separator(*rit))
      break;
    else if (*rit == '.' && last_dot == filename.end())
      last_dot = rit.base()-1;
  }

  for (std::string::const_iterator it(rit.base()); it!=filename.end(); ++it) {
    if (it == last_dot)
      break;
    else
      result.push_back(*it);
  }

  return result;
}

std::string get_file_title_with_path(const std::string& filename)
{
  std::string::const_reverse_iterator rit;

  // search for the first dot from the end of the string
  for (rit=filename.rbegin(); rit!=filename.rend(); ++rit) {
    if (is_path_separator(*rit))
      return filename;
    else if (*rit == '.')
      break;
  }

  if (rit != filename.rend())
    return filename.substr(0, rit.base() - filename.begin() - 1);
  else
    return filename;
}

std::string join_path(const std::string& path, const std::string& file)
{
  std::string result(path);

  // Add a separator at the end if it is necessay
  if (!result.empty() && !is_path_separator(*(result.end()-1)))
    result.push_back(path_separator);

  // Add the file
  result += file;
  return result;
}

std::string remove_path_separator(const std::string& path)
{
  std::string result(path);

  // Erase all trailing separators
  while (!result.empty() && is_path_separator(*(result.end()-1)))
    result.erase(result.end()-1);

  return result;
}

std::string fix_path_separators(const std::string& filename)
{
  std::string result(filename);

  // Replace any separator with the system path separator.
  std::replace_if(result.begin(), result.end(),
                  is_path_separator, path_separator);

  return result;
}

std::string normalize_path(const std::string& filename)
{
  std::string fn = base::get_canonical_path(filename);
  fn = base::fix_path_separators(fn);
  return fn;
}

bool has_file_extension(const std::string& filename, const base::paths& extensions)
{
  if (!filename.empty()) {
    const std::string ext = get_file_extension(filename);
    for (const auto& e : extensions)
      if (utf8_icmp(ext, e) == 0)
        return true;
  }
  return false;
}

int compare_filenames(const std::string& a, const std::string& b)
{
  utf8_const_iterator a_begin(a.begin()), a_end(a.end());
  utf8_const_iterator b_begin(b.begin()), b_end(b.end());
  utf8_const_iterator a_it(a_begin);
  utf8_const_iterator b_it(b_begin);

  for (; a_it != a_end && b_it != b_end; ) {
    int a_chr = *a_it;
    int b_chr = *b_it;

    if ((a_chr >= '0') && (a_chr <= '9') && (b_chr >= '0') && (b_chr <= '9')) {
      utf8_const_iterator a_it2 = a_it;
      utf8_const_iterator b_it2 = b_it;

      while (a_it2 != a_end && (*a_it2 >= '0') && (*a_it2 <= '9')) ++a_it2;
      while (b_it2 != b_end && (*b_it2 >= '0') && (*b_it2 <= '9')) ++b_it2;

      int a_num = std::strtol(std::string(a_it, a_it2).c_str(), NULL, 10);
      int b_num = std::strtol(std::string(b_it, b_it2).c_str(), NULL, 10);
      if (a_num != b_num)
        return a_num - b_num < 0 ? -1: 1;

      a_it = a_it2;
      b_it = b_it2;
    }
    else if (is_path_separator(a_chr) && is_path_separator(b_chr)) {
      ++a_it;
      ++b_it;
    }
    else {
      a_chr = std::tolower(a_chr);
      b_chr = std::tolower(b_chr);

      if (a_chr != b_chr)
        return a_chr - b_chr < 0 ? -1: 1;

      ++a_it;
      ++b_it;
    }
  }

  if (a_it == a_end && b_it == b_end)
    return 0;
  else if (a_it == a_end)
    return -1;
  else
    return 1;
}

} // namespace base
