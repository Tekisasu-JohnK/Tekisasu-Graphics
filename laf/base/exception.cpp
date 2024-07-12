// LAF Base Library
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/exception.h"
#include "base/string.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

namespace base {

using namespace std;

Exception::Exception() throw()
{
}

Exception::Exception(const char* format, ...) throw()
{
  try {
    if (!std::strchr(format, '%')) {
      m_msg = format;
    }
    else {
      va_list ap;
      va_start(ap, format);
      m_msg = base::string_vprintf(format, ap);
      va_end(ap);
    }
  }
  catch (...) {
    // No throw
  }
}

Exception::Exception(const std::string& msg) throw()
{
  try {
    m_msg = msg;
  }
  catch (...) {
    // No throw
  }
}

Exception::~Exception() throw()
{
}

void Exception::setMessage(const char* msg) throw()
{
  try {
    m_msg = msg;
  }
  catch (...) {
    // No throw
  }
}

const char* Exception::what() const throw()
{
  return m_msg.c_str();
}

} // namespace base
