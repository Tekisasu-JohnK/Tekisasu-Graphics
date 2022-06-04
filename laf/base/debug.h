// LAF Base Library
// Copyright (C) 2019-2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_DEBUG_H_INCLUDED
#define BASE_DEBUG_H_INCLUDED
#pragma once

#include "base/base.h"

int base_assert(const char* condition, const char* file, int lineNum) LAF_ANALYZER_NORETURN;
void base_trace(const char* msg, ...);

#ifdef __cplusplus
  #include <sstream>
  #include <string>

  template<typename Arg>
  void base_args_to_string_step(std::stringstream& s, Arg&& arg) {
    s << std::forward<Arg>(arg);
  }

  template<typename Arg, typename... Args>
  void base_args_to_string_step(std::stringstream& s, Arg&& arg, Args&&... args) {
    s << std::forward<Arg>(arg) << ' ';
    base_args_to_string_step<Args...>(s, std::forward<Args>(args)...);
  }

  template<typename... Args>
  std::string base_args_to_string(Args&&... args) {
    std::stringstream s;
    base_args_to_string_step(s, std::forward<Args>(args)...);
    return s.str();
  }

  template<typename... Args>
  void base_trace_args(Args&&... args) {
    std::string s = base_args_to_string(std::forward<Args>(args)...);
    s.push_back('\n');
    base_trace(s.c_str());
  }
#endif

#undef ASSERT
#undef TRACE
#undef TRACEARGS

#ifdef _DEBUG
  #if LAF_WINDOWS
    #include <crtdbg.h>
    #define base_break() _CrtDbgBreak()
  #else
    #include <signal.h>
    #define base_break() raise(SIGTRAP)
  #endif

  #define ASSERT(condition) {                                             \
    if (!(condition)) {                                                   \
      if (base_assert(#condition, __FILE__, __LINE__)) {                  \
        base_break();                                                     \
      }                                                                   \
    }                                                                     \
  }

  #define TRACE base_trace
  #define TRACEARGS base_trace_args
#else
  #define ASSERT(condition)
  #define TRACE(...)
  #define TRACEARGS(...)
#endif

#endif
