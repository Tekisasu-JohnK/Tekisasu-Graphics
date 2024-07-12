// LAF Base Library
// Copyright (c) 2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/platform.h"

#include "base/file_handle.h"

#include <cstdio>

namespace base {

std::map<std::string, std::string> get_linux_release_info(const std::string& fn)
{
  std::map<std::string, std::string> values;

  const FileHandle f(open_file(fn, "r"));
  if (!f)
    return values;

  std::vector<char> buf(1024);
  std::string value;

  while (std::fgets(buf.data(), buf.size(), f.get())) {
    for (auto i=buf.begin(), end=buf.end(); i != end; ++i) {
      // Commented line
      if (*i == '#')
        break;
      // Ignore initial whitespace
      if (*i == ' ')
        continue;
      // Read the key
      if (*i >= 'A' && *i <= 'Z') {
        auto j = i;
        while (j != end && ((*j >= 'A' && *j <= 'Z') ||
                            (*j >= '0' && *j <= '9') || (*j == '_'))) {
          ++j;
        }

        const std::string key(i, j);

        // Ignore white space between "KEY ... ="
        while (j != end && *j == ' ')
          ++j;
        if (j != end && *j == '=') {
          ++j;          // Skip '='
          // Ignore white space between "KEY= ... VALUE"
          while (j != end && *j == ' ')
            ++j;

          value.clear();

          if (j != end) {
            const char quote = *j;
            if (quote == '\'' || quote == '\"') {
              ++j;
              while (j != end && *j != quote) {
                if (*j == '\\') {
                  ++j;
                  if (j == end)
                    break;
                }
                value.push_back(*j);
                ++j;
              }
            }
            else {
              while (j != end && (*j != ' ' &&
                                  *j != '\r' &&
                                  *j != '\n')) {
                value.push_back(*j);
                ++j;
              }
            }
          }

          values[key] = value;
        }
        break; // Next line
      }
      // Unexpected character in this line
      break;
    }

    // Too many key-values
    if (values.size() > 4096)
      break;
  }

  return values;
}

} // namespace base
