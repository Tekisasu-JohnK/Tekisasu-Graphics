// LAF OS Library
// Copyright (C) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_RESIZE_SURFACE_INCLUDED
#define OS_SKIA_RESIZE_SURFACE_INCLUDED
#pragma once

#include "os/display.h"
#include "os/skia/skia_surface.h"

namespace os {

class ResizeSurface {
public:
  ResizeSurface();
  ~ResizeSurface();

  ResizeSurface(const ResizeSurface&) = delete;
  ResizeSurface& operator=(const ResizeSurface&) = delete;

  void create(Display* display);
  void reset();
  void draw(Display* display);

  operator bool() { return m_snapshot != nullptr; }

private:
  SkiaSurface* m_snapshot;
};

} // namespace os

#endif
