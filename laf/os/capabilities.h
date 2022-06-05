// LAF OS Library
// Copyright (c) 2018-2021  Igara Studio S.A.
// Copyright (C) 2012-2015  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_CAPABILITIES_H_INCLUDED
#define OS_CAPABILITIES_H_INCLUDED
#pragma once

namespace os {

  enum class Capabilities {
    // Supports the creation of multiple os::Window. If this is not
    // set, the system supports just one display, like a phone device.
    MultipleWindows = 1,

    // When os::Window can be resized.
    CanResizeWindow = 2,

    // When we can change the window scale.
    WindowScale = 4,

    // When we can set the mouse cursor with a custom os::Surface
    // using os::Window::makeMouseCursor()
    CustomMouseCursor = 8,

    // When GPU acceleration can be turned on.
    // TODO this is being developed
    GpuAccelerationSwitch = 16,

    // When the platform support changing the color space of the
    // window.
    ColorSpaces = 32,

    // Windows & Linux allow to the programmer to start the
    // drag-window-to-resize-it loop from a os::Event:MouseDown, but
    // macOS doesn't (macOS supports only start moving the window).
    CanStartWindowResize = 64
  };

} // namespace os

#endif
