// LAF OS Library
// Copyright (C) 2021  Igara Studio S.A.
// Copyright (C) 2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_SYSTEM_H
#define OS_X11_SYSTEM_H
#pragma once

#include "os/common/system.h"
#include "os/x11/keys.h"
#include "os/x11/screen.h"
#include "os/x11/x11.h"

namespace os {

class SystemX11 : public CommonSystem {
public:
  ~SystemX11();

  bool isKeyPressed(KeyScancode scancode) override {
    return x11_is_key_pressed(scancode);
  }

  int getUnicodeFromScancode(KeyScancode scancode) override {
    return x11_get_unicode_from_scancode(scancode);
  }

  CursorRef getNativeCursor(NativeCursor cursor);

  CursorRef makeCursor(const Surface* surface,
                       const gfx::Point& focus,
                       const int scale) override;

  gfx::Point mousePosition() const override {
    int rootx, rooty, x, y;
    unsigned int mask;
    ::Display* display = X11::instance()->display();
    ::Window root = XDefaultRootWindow(display);
    ::Window child;
    if (!XQueryPointer(display, root, &root, &child, &rootx, &rooty, &x, &y, &mask)) {
      rootx = rooty = 0;
    }
    return gfx::Point(rootx, rooty);
  }

  void setMousePosition(const gfx::Point& screenPosition) override {
    // TODO
  }

  gfx::Color getColorFromScreen(const gfx::Point& screenPosition) const override {
#if 0
    ::Display* display = X11::instance()->display();
    int screen = XDefaultScreen(display);
    ::Window root = XDefaultRootWindow(display);

    // TODO XGetImage() crashes with a BadMatch error
    XImage* image = XGetImage(display, root,
                              screenPosition.x,
                              screenPosition.y, 1, 1, AllPlanes, ZPixmap);
    if (image) {
      XColor color;
      color.pixel = XGetPixel(image, screenPosition.x, screenPosition.y);
      XFree(image);

      XQueryColor(display, XDefaultColormap(display, screen), &color);

      // Each red/green/blue channel is 16-bit, so we have to convert to 8-bit.
      return gfx::rgba(color.red>>8, color.green>>8, color.blue>>8);
    }
    else
#endif
      return gfx::ColorNone;
  }

  ScreenRef mainScreen() override {
    return make_ref<ScreenX11>(
      XDefaultScreen(X11::instance()->display()));
  }

  void listScreens(ScreenList& list) override {
    const int nscreen = XScreenCount(X11::instance()->display());
    for (int screen=0; screen<nscreen; screen++)
      list.push_back(make_ref<ScreenX11>(screen));
  }

};

} // namespace os

#endif
