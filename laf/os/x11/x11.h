// LAF OS Library
// Copyright (C) 2020-2022  Igara Studio S.A.
// Copyright (C) 2016-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_X11_INCLUDED
#define OS_X11_X11_INCLUDED
#pragma once

#include "base/debug.h"
#include "gfx/color_space.h"    // Include here avoid error with None
#include "os/event_queue.h"

#include <X11/Xlib.h>

#include <memory>

namespace os {

class XInput;

class X11 {
  static X11* m_instance;
public:
  static X11* instance();

  X11();
  ~X11();

  ::Display* display() const { return m_display; }
  ::XIM xim() const { return m_xim; }
  XInput* xinput();

  std::string userDefinedTablet() { return m_userDefinedTablet; }
  void setUserDefinedTablet(const std::string& str) {
    m_userDefinedTablet = str;
  }

private:
  ::Display* m_display;
  ::XIM m_xim;
  std::unique_ptr<XInput> m_xinput;
  std::string m_userDefinedTablet;
};

// User-defined string to detect a stylys/pen because it looks like
// Linux doesn't offer a flag/device type to detect them (?)
void x11_set_user_defined_string_to_detect_stylus(const std::string& str);

} // namespace os

#endif
