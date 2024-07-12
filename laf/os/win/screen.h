// LAF OS Library
// Copyright (c) 2020-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_SCREEN_H
#define OS_WIN_SCREEN_H
#pragma once

#include "os/screen.h"
#include "os/win/color_space.h"

namespace os {

class ScreenWin : public Screen {
public:
  ScreenWin(HMONITOR hmonitor) : m_monitor(hmonitor) {
    MONITORINFOEXA mi;
    memset((void*)&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoA(m_monitor, &mi)) {
      auto rc = mi.rcMonitor;
      auto wa = mi.rcWork;

      m_bounds.x = rc.left;
      m_bounds.y = rc.top;
      m_bounds.w = rc.right - rc.left;
      m_bounds.h = rc.bottom - rc.top;

      m_workarea.x = wa.left;
      m_workarea.y = wa.top;
      m_workarea.w = wa.right - rc.left;
      m_workarea.h = wa.bottom - rc.top;

      m_isMainScreen = (mi.dwFlags & MONITORINFOF_PRIMARY ? true: false);
    }
  }
  bool isMainScreen() const override { return m_isMainScreen; }
  gfx::Rect bounds() const override { return  m_bounds; }
  gfx::Rect workarea() const override { return m_workarea; }
  os::ColorSpaceRef colorSpace() const override {
    return get_hmonitor_colorspace(m_monitor);
  }
  void* nativeHandle() const override {
    return reinterpret_cast<void*>(m_monitor);
  }
private:
  HMONITOR m_monitor;
  gfx::Rect m_bounds;
  gfx::Rect m_workarea;
  bool m_isMainScreen = false;
};

} // namespace os

#endif
