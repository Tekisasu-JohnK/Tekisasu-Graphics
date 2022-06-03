// LAF OS Library
// Copyright (C) 2018-2019  Igara Studio S.A.
// Copyright (C) 2016-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_WINDOW_INCLUDED
#define OS_X11_WINDOW_INCLUDED
#pragma once

#include "base/time.h"
#include "gfx/color_space.h"    // Include here avoid error with None
#include "gfx/fwd.h"
#include "gfx/size.h"
#include "os/color_space.h"
#include "os/event.h"
#include "os/native_cursor.h"
#include "os/surface_list.h"

#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cstring>
#include <string>

namespace os {

class Surface;

class X11Window {
public:
  X11Window(::Display* display, int width, int height, int scale);
  ~X11Window();

  void queueEvent(Event& ev);

  os::ColorSpacePtr colorSpace() const;

  int scale() const { return m_scale; }
  void setScale(const int scale);

  void setTitle(const std::string& title);
  void setIcons(const SurfaceList& icons);

  gfx::Size clientSize() const;
  gfx::Size restoredSize() const;
  void captureMouse();
  void releaseMouse();
  void setMousePosition(const gfx::Point& position);
  void invalidateRegion(const gfx::Region& rgn);
  bool setNativeMouseCursor(NativeCursor cursor);
  bool setNativeMouseCursor(const os::Surface* surface,
                            const gfx::Point& focus,
                            const int scale);

  ::Display* x11display() const { return m_display; }
  ::Window handle() const { return m_window; }
  ::GC gc() const { return m_gc; }

  void setTranslateDeadKeys(bool state) {
    // TODO
  }

  void processX11Event(XEvent& event);
  static X11Window* getPointerFromHandle(Window handle);

protected:
  virtual void onQueueEvent(Event& event) = 0;
  virtual void onPaint(const gfx::Rect& rc) = 0;
  virtual void onResize(const gfx::Size& sz) = 0;

private:
  void setWMClass(const std::string& res_class);
  bool setX11Cursor(::Cursor xcursor);
  static void addWindow(X11Window* window);
  static void removeWindow(X11Window* window);

  ::Display* m_display;
  ::Window m_window;
  ::GC m_gc;
  ::Cursor m_cursor;
  ::XcursorImage* m_xcursorImage;
  ::XIC m_xic;
  int m_scale;
  gfx::Point m_lastMousePos;
  gfx::Size m_lastClientSize;

  // Double-click info
  Event::MouseButton m_doubleClickButton;
  base::tick_t m_doubleClickTick;
};

} // namespace os

#endif
