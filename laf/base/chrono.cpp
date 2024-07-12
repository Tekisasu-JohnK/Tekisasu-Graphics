// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/chrono.h"

#if LAF_WINDOWS
  #include "base/chrono_win32.h"
#else
  #include "base/chrono_unix.h"
#endif

namespace base {

  Chrono::Chrono() : m_impl(new ChronoImpl) {
  }

  Chrono::~Chrono() {
    delete m_impl;
  }

  void Chrono::reset() {
    m_impl->reset();
  }

  double Chrono::elapsed() const {
    return m_impl->elapsed();
  }

} // namespace base
