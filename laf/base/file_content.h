// LAF Base Library
// Copyright (C) 2018-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_FILE_CONTENT_H_INCLUDED
#define BASE_FILE_CONTENT_H_INCLUDED
#pragma once

#include "base/buffer.h"

#include <string>

namespace base {

  buffer read_file_content(const std::string& filename);
  void write_file_content(const std::string& filename, const buffer& buf);
  void write_file_content(const std::string& filename, const uint8_t* data, size_t size);

} // namespace base

#endif
