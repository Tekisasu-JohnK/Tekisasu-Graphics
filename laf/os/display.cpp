// LAF OS Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/display.h"

#include "gfx/rect.h"
#include "gfx/region.h"

namespace os {

gfx::Rect Display::bounds() const
{
  return gfx::Rect(0, 0, width(), height());
}

void Display::invalidate()
{
  invalidateRegion(gfx::Region(bounds()));
}

} // namespace os
