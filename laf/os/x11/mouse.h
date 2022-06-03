// LAF OS Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_MOUSE_INCLUDED
#define OS_X11_MOUSE_INCLUDED
#pragma once

#include "os/event.h"
#include <X11/X.h>

namespace os {

  inline Event::MouseButton get_mouse_button_from_x(int button) {
    switch (button) {
      case Button1: return Event::LeftButton;
      case Button2: return Event::MiddleButton;
      case Button3:  return Event::RightButton;
      case 8:        return Event::X1Button;
      case 9:        return Event::X2Button;
    }
    TRACE("Unknown Button %d\n", button);
    return Event::NoneButton;
  }

} // namespace os

#endif
