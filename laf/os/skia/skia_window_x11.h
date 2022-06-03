// LAF OS Library
// Copyright (C) 2016-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_WINDOW_X11_INCLUDED
#define OS_SKIA_SKIA_WINDOW_X11_INCLUDED
#pragma once

#include "base/disable_copying.h"
#include "gfx/size.h"
#include "os/native_cursor.h"
#include "os/skia/resize_surface.h"
#include "os/x11/window.h"

#include <string>
#include <vector>

namespace os {

class EventQueue;
class SkiaDisplay;

class SkiaWindow : public X11Window {
public:
  enum class Backend { NONE, GL };

  SkiaWindow(EventQueue* queue, SkiaDisplay* display,
             int width, int height, int scale);
  ~SkiaWindow();

  void setVisible(bool visible);
  void maximize();
  bool isMaximized() const;
  bool isMinimized() const;

  std::string getLayout() { return ""; }
  void setLayout(const std::string& layout) { }

private:
  void onQueueEvent(Event& ev) override;
  void onPaint(const gfx::Rect& rc) override;
  void onResize(const gfx::Size& sz) override;

  EventQueue* m_queue;
  SkiaDisplay* m_display;
  ResizeSurface m_resizeSurface;
  std::vector<uint8_t> m_buffer;

  DISABLE_COPYING(SkiaWindow);
};

} // namespace os

#endif
