// LAF Base Library
// Copyright (c) 2022 Igara Studio S.A.
// Copyright (c) 2015-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_BASE64_H_INCLUDED
#define BASE_BASE64_H_INCLUDED
#pragma once

#include "base/buffer.h"

#include <string>

namespace base {

void encode_base64(const char* input, size_t n, std::string& output);
void decode_base64(const char* input, size_t n, buffer& output);

inline void encode_base64(const buffer& input, std::string& output) {
  if (!input.empty())
    encode_base64((const char*)&input[0], input.size(), output);
}

inline std::string encode_base64(const buffer& input) {
  std::string output;
  if (!input.empty())
    encode_base64((const char*)&input[0], input.size(), output);
  return output;
}

inline std::string encode_base64(const std::string& input) {
  std::string output;
  if (!input.empty())
    encode_base64((const char*)input.c_str(), input.size(), output);
  return output;
}

inline void decode_base64(const std::string& input, buffer& output) {
  if (!input.empty())
    decode_base64(input.c_str(), input.size(), output);
}

inline buffer decode_base64(const std::string& input) {
  buffer output;
  if (!input.empty())
    decode_base64(input.c_str(), input.size(), output);
  return output;
}

inline std::string decode_base64s(const std::string& input) {
  if (input.empty())
    return std::string();
  buffer tmp;
  decode_base64(input.c_str(), input.size(), tmp);
  return std::string((const char*)&tmp[0], tmp.size());
}

inline buffer decode_base64(const buffer& input) {
  buffer output;
  if (!input.empty())
    decode_base64((const char*)&input[0], input.size(), output);
  return output;
}

} // namespace base

#endif
