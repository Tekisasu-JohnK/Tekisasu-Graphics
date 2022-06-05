// LAF OS Library
// Copyright (C) 2018-2021  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/skia/skia_window.h"

#include "base/debug.h"
#include "os/window_spec.h"
#include "os/event.h"
#include "os/event_queue.h"
#include "os/skia/skia_surface.h"
#include "os/system.h"

namespace os {

SkiaWindow::SkiaWindow(const WindowSpec& spec)
  : SkiaWindowPlatform(spec)
  , m_nativeCursor(NativeCursor::Arrow)
{
  setScale(spec.scale());
  setCursor(m_nativeCursor);
  initializeSurface();
}

int SkiaWindow::width() const
{
  return clientSize().w;
}

int SkiaWindow::height() const
{
  return clientSize().h;
}

NativeCursor SkiaWindow::nativeCursor()
{
  return m_nativeCursor;
}

bool SkiaWindow::setCursor(NativeCursor cursor)
{
  m_nativeCursor = cursor;
  return SkiaWindowPlatform::setCursor(cursor);
}

} // namespace os
