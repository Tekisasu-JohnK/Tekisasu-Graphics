// LAF OS Library
// Copyright (C) 2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_CURSOR_H
#define OS_X11_CURSOR_H
#pragma once

#include "os/cursor.h"
#include "os/x11/x11.h"

#include <X11/Xlib.h>

namespace os {

class CursorX11 : public Cursor {
public:
  CursorX11(::Cursor xcursor) : m_xcursor(xcursor) { }
  ~CursorX11() {
    if (m_xcursor != None) {
      auto x11 = X11::instance();
      ASSERT(x11);
      if (x11)
        XFreeCursor(x11->display(), m_xcursor);
    }
  }

  void* nativeHandle() override {
    return (void*)m_xcursor;
  }

private:
  ::Cursor m_xcursor;
};

} // namespace os

#endif
