// LAF Base Library
// Copyright (c) 2023-2024 Igara Studio S.A.
// Copyright (c) 2015-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_PROCESS_H_INCLUDED
#define BASE_PROCESS_H_INCLUDED
#pragma once

#include "base/ints.h"

#include <string>

namespace base {

  #ifdef LAF_WINDOWS
    using pid = uint32_t; // DWORD
  #else
    using pid = int;
  #endif

  pid get_current_process_id();

  bool is_process_running(pid pid);

  std::string get_process_name(pid pid);

} // namespace base

#endif
