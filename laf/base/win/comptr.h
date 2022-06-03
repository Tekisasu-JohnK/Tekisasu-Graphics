// LAF Base Library
// Copyright (c) 2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_WIN_COMPTR_H_INCLUDED
#define BASE_WIN_COMPTR_H_INCLUDED
#pragma once

#if !defined(_WIN32)
  #error This header file can be used only on Windows platform
#endif

#include "base/disable_copying.h"

namespace base {

  template<class T>
  class ComPtr {
  public:
    ComPtr() : m_ptr(nullptr) { }
    ~ComPtr() { reset(); }
    T** operator&() { return &m_ptr; }
    T* operator->() { return m_ptr; }

    T* get() {
      return m_ptr;
    }

    void reset() {
      if (m_ptr) {
        m_ptr->Release();
        m_ptr = nullptr;
      }
    }

  private:
    T* m_ptr;

    DISABLE_COPYING(ComPtr);
  };

} // namespace base

#endif
