// LAF OS Library
// Copyright (c) 2020-2022  Igara Studio S.A.
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

  inline int get_x_mouse_button_from_event(Event::MouseButton button) {
    switch (button) {
      case Event::NoneButton:   return 0;
      case Event::LeftButton:   return Button1;
      case Event::MiddleButton: return Button2;
      case Event::RightButton:  return Button3;
      case Event::X1Button:     return 8;
      case Event::X2Button:     return 9;
    }
    TRACE("Unknown Button %d\n", button);
    return 0;
  }

} // namespace os

#endif
