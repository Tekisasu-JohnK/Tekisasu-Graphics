// LAF OS Library
// Copyright (c) 2020-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_SCREEN_H
#define OS_X11_SCREEN_H
#pragma once

#include "os/screen.h"
#include "os/x11/x11.h"

#include <X11/Xatom.h>

namespace os {

class ScreenX11 : public Screen {
public:
  ScreenX11(int screen) {
    auto x11 = X11::instance();
    auto x11display = x11->display();

    m_bounds.w = XDisplayWidth(x11display, screen);
    m_bounds.h = XDisplayHeight(x11display, screen);

    ::Window root = XDefaultRootWindow(x11display);
    Atom _NET_WORKAREA = XInternAtom(x11display, "_NET_WORKAREA", False);

    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned long* prop;

    int res = XGetWindowProperty(x11display, root,
                                 _NET_WORKAREA,
                                 0, 4,
                                 False, XA_CARDINAL,
                                 &actual_type, &actual_format,
                                 &nitems, &bytes_after,
                                 (unsigned char**)&prop);
    if (res == Success && nitems == 4) {
      m_workarea.x = prop[0];
      m_workarea.y = prop[1];
      m_workarea.w = prop[2];
      m_workarea.h = prop[3];
      XFree(prop);
    }
    else {
      m_workarea = m_bounds;
    }
  }
  bool isMainScreen() const override {
    return (m_screen == XDefaultScreen(X11::instance()->display()));
  }
  gfx::Rect bounds() const override { return m_bounds; }
  gfx::Rect workarea() const override { return m_workarea; }
  os::ColorSpaceRef colorSpace() const override {
    // TODO get screen color space
    return os::instance()->makeColorSpace(gfx::ColorSpace::MakeSRGB());
  }
  void* nativeHandle() const override {
    return reinterpret_cast<void*>(m_screen);
  }
private:
  int m_screen;
  gfx::Rect m_bounds;
  gfx::Rect m_workarea;
};

} // namespace os

#endif
