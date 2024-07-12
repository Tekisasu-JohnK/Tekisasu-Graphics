// LAF OS Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SCREEN_H_INCLUDED
#define OS_SCREEN_H_INCLUDED
#pragma once

#include "gfx/rect.h"
#include "os/color_space.h"

#include <vector>

namespace os {

  class Screen;
  using ScreenRef = Ref<Screen>;
  using ScreenList = std::vector<ScreenRef>;

  // A display or window to show graphics.
  class Screen : public RefCount {
  public:
    virtual ~Screen() { }

    // Returns true if it's the main screen.
    virtual bool isMainScreen() const = 0;

    // Returns the size of the whole screen.
    virtual gfx::Rect bounds() const = 0;

    // Returns the area of the screen without the task bar, i.e. the
    // desktop area, the maximum area of a window when it's maximized
    // (but not in full screen).
    virtual gfx::Rect workarea() const = 0;

    // Returns the color space of this screen.
    virtual os::ColorSpaceRef colorSpace() const = 0;

    // Returns the HMONITOR (Windows), NSScreen* (macOS), or screen number (X11).
    virtual void* nativeHandle() const = 0;
  };

} // namespace os

#endif
