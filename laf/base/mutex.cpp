// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/mutex.h"

#if LAF_WINDOWS
  #include "base/mutex_win32.h"
#else
  #include "base/mutex_pthread.h"
#endif

namespace base {

mutex::mutex()
  : m_impl(new mutex_impl)
{
}

mutex::~mutex()
{
  delete m_impl;
}

void mutex::lock()
{
  return m_impl->lock();
}

bool mutex::try_lock()
{
  return m_impl->try_lock();
}

void mutex::unlock()
{
  return m_impl->unlock();
}

} // namespace base
