// LAF OS Library
// Copyright (C) 2019-2020  Igara Studio S.A.
// Copyright (C) 2012-2013  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SCOPED_HANDLE_H_INCLUDED
#define OS_SCOPED_HANDLE_H_INCLUDED
#pragma once

namespace os {

  template<typename T>
  class ScopedHandle {
  public:
    ScopedHandle(T* handle) : m_handle(handle) { }
    ScopedHandle(ScopedHandle&& that) {
      m_handle = that.m_handle;
      that.m_handle = nullptr;
    }
    ~ScopedHandle() {
      if (m_handle)
        m_handle->dispose();
    }

    T* operator->() { return m_handle; }
    operator T*() { return m_handle; }

    const T* operator->() const { return m_handle; }
    operator const T*() const { return m_handle; }

  private:
    T* m_handle;

    // Cannot copy
    ScopedHandle(const ScopedHandle&);
    ScopedHandle& operator=(const ScopedHandle&);
  };

} // namespace os

#endif
