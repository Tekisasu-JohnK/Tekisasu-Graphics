// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2016-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_X11_INCLUDED
#define OS_X11_X11_INCLUDED
#pragma once

#include "base/debug.h"
#include "gfx/color_space.h"    // Include here avoid error with None
#include "os/x11/xinput.h"

#include <X11/Xlib.h>

namespace os {

class X11 {
  static X11* m_instance;
public:
  static X11* instance();

  X11() {
    ASSERT(m_instance == nullptr);
    m_instance = this;

    // TODO We shouldn't need to call this function (because we
    // shouldn't be using the m_display from different threads), but
    // it might be necessary?
    // https://github.com/aseprite/aseprite/issues/1962
    XInitThreads();

    m_display = XOpenDisplay(nullptr);
    m_xim = XOpenIM(m_display, nullptr, nullptr, nullptr);
    if (m_display)
      m_xinput.load(m_display);
  }

  ~X11() {
    ASSERT(m_instance == this);
    if (m_xim) {
      XCloseIM(m_xim);
    }
    if (m_display) {
      m_xinput.unload(m_display);
      XCloseDisplay(m_display);
    }
    m_instance = nullptr;
  }

  ::Display* display() const { return m_display; }
  ::XIM xim() const { return m_xim; }
  XInput& xinput() { return m_xinput; }

private:
  ::Display* m_display;
  ::XIM m_xim;
  XInput m_xinput;
};

} // namespace os

#endif
