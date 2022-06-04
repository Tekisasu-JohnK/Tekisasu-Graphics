// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _DEBUG

#include "base/debug.h"

#include "base/convert_to.h"
#include "base/string.h"

#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#if LAF_WINDOWS
  #include <windows.h>
#endif

int base_assert(const char* condition, const char* file, int lineNum)
{
#if LAF_WINDOWS

  std::vector<wchar_t> buf(MAX_PATH);
  GetModuleFileNameW(NULL, &buf[0], MAX_PATH);

  int ret = _CrtDbgReportW(_CRT_ASSERT,
    base::from_utf8(file).c_str(),
    lineNum,
    &buf[0],
    base::from_utf8(condition).c_str());

  return (ret == 1 ? 1: 0);

#else

  std::string text = file;
  text += ":";
  text += base::convert_to<std::string>(lineNum);
  text += ": Assertion failed: ";
  text += condition;
  std::cerr << text << std::endl;
  std::abort();
  return 1;

#endif
}

void base_trace(const char* msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  char buf[4096];
  vsprintf(buf, msg, ap);
  va_end(ap);

#if LAF_WINDOWS
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL, buf);
#endif

  std::cerr << buf << std::flush;
}

#endif
