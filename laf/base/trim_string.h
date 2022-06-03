// LAF Base Library
// Copyright (c) 2001-2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_TRIM_STRING_H_INCLUDED
#define BASE_TRIM_STRING_H_INCLUDED
#pragma once

#include <cctype>
#include <string>

namespace base {

template<typename Pred>
void trim_string(const std::string& input,
                 std::string& output,
                 const Pred& pred)
{
  int i, j;

  for (i=0; i<(int)input.size(); ++i)
    if (!pred(input.at(i)))
      break;

  for (j=(int)input.size()-1; j>i; --j)
    if (!pred(input.at(j)))
      break;

  if (i < j)
    output = input.substr(i, j - i + 1);
  else
    output = std::string();
}

inline void trim_string(const std::string& input,
                        std::string& output) {
  trim_string<int(int)>(input, output, std::isspace);
}

}

#endif
