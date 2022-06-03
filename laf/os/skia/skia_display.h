// LAF OS Library
// Copyright (c) 2018-2020  Igara Studio S.A.
// Copyright (c) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_DISPLAY_INCLUDED
#define OS_SKIA_SKIA_DISPLAY_INCLUDED
#pragma once

#include "os/display.h"
#include "os/native_cursor.h"
#include "os/skia/skia_color_space.h"
#include "os/skia/skia_window.h"

namespace os {

class SkiaSurface;

class SkiaDisplay : public Display {
public:
  SkiaDisplay(int width, int height, int scale);

  bool isInitialized() const { return m_initialized; }
  void setSkiaSurface(SkiaSurface* surface);
  void resetSkiaSurface();

  void resize(const gfx::Size& size);
  void dispose() override;

  // Returns the real and current display's size (without scale applied).
  int width() const override;
  int height() const override;

  // Returns the display when it was not maximized.
  int originalWidth() const override;
  int originalHeight() const override;

  int scale() const override;
  void setScale(int scale) override;

  // Returns the main surface to draw into this display.
  // You must not dispose this surface.
  Surface* getSurface() override;

  // Invalidate parts of the OS window to be updated in the future
  // with paint messages.
  void invalidateRegion(const gfx::Region& rgn) override;

  void maximize() override;
  bool isMaximized() const override;
  bool isMinimized() const override;
  void setTitle(const std::string& title) override;
  void setIcons(const SurfaceList& icons) override;
  NativeCursor nativeMouseCursor() override;
  bool setNativeMouseCursor(NativeCursor cursor) override;
  bool setNativeMouseCursor(const os::Surface* surface,
                            const gfx::Point& focus,
                            const int scale) override;
  void setMousePosition(const gfx::Point& position) override;
  void captureMouse() override;
  void releaseMouse() override;

  std::string getLayout() override;
  void setLayout(const std::string& layout) override;

  void setInterpretOneFingerGestureAsMouseMovement(bool state) override;

  void setTranslateDeadKeys(bool state);

  os::ColorSpacePtr colorSpace() const override { return m_colorSpace; }
  void setColorSpace(const os::ColorSpacePtr& colorSpace);
  os::ColorSpacePtr currentMonitorColorSpace() const;

  void onTabletAPIChange();

  // Returns the HWND on Windows.
  NativeHandle nativeHandle() override;

private:
  // Flag used to avoid accessing to an invalid m_surface in the first
  // SkiaDisplay::resize() call when the SkiaWindow is created (as the
  // window is created, it send a first resize event.)
  bool m_initialized;
  SkiaWindow m_window;
  SkiaSurface* m_surface;
  os::ColorSpacePtr m_colorSpace;
  bool m_customSurface;
  NativeCursor m_nativeCursor;
};

} // namespace os

#endif
