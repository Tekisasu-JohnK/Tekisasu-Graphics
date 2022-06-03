// LAF Gfx Library
// Copyright (C) 2019 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gfx/region.h"

namespace gfx {

inline Rect to_rect(const SkIRect& rc)
{
  return Rect(rc.x(), rc.y(), rc.width(), rc.height());
}

Region::Region()
{
}

Region::Region(const Region& copy)
  : m_region(copy.m_region)
{
}

Region::Region(const Rect& rect)
  : m_region(SkIRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h))
{
}

Region& Region::operator=(const Rect& rect)
{
  m_region.setRect(SkIRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h));
  return *this;
}

Region& Region::operator=(const Region& copy)
{
  m_region = copy.m_region;
  return *this;
}

Region::iterator Region::begin()
{
  iterator it;
  it.m_it = SkRegion::Iterator(m_region);
  return it;
}

Region::iterator Region::end()
{
  return iterator();
}

Region::const_iterator Region::begin() const
{
  iterator it;
  it.m_it = SkRegion::Iterator(m_region);
  return it;
}

Region::const_iterator Region::end() const
{
  return iterator();
}

Rect Region::bounds() const
{
  return to_rect(m_region.getBounds());
}

Region::Overlap Region::contains(const Rect& rect) const
{
  auto rc = SkIRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h);
  if (m_region.contains(rc))
    return In;
  else if (m_region.intersects(rc))
    return Part;
  else
    return Out;
}

} // namespace gfx
