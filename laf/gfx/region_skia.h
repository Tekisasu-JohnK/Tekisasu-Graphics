// LAF Gfx Library
// Copyright (C) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_REGION_SKIA_H_INCLUDED
#define GFX_REGION_SKIA_H_INCLUDED
#pragma once

#include "gfx/point.h"
#include "gfx/rect.h"

#include <vector>

#include "include/core/SkRegion.h"

namespace gfx {

  template<typename T> class PointT;

  class Region;

  namespace details {

    using Region = SkRegion;

    template<typename T>
    class RegionIterator {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T*;
      using reference = T&;

      RegionIterator() { }
      RegionIterator(const RegionIterator& o) : m_it(o.m_it) { }
      template<typename T2>
      RegionIterator(const RegionIterator<T2>& o) : m_it(o.m_it) { }
      RegionIterator& operator=(const RegionIterator& o) { m_it = o.m_it; return *this; }
      RegionIterator& operator++() { m_it.next(); return *this; }
      bool operator==(const RegionIterator& o) const {
        return (m_it.done() == o.m_it.done());
      }
      bool operator!=(const RegionIterator& o) const {
        return (m_it.done() != o.m_it.done());
      }
      reference operator*() {
        const SkIRect& rc = m_it.rect();
        m_rect.x = rc.x();
        m_rect.y = rc.y();
        m_rect.w = rc.width();
        m_rect.h = rc.height();
        return m_rect;
      }
    private:
      SkRegion::Iterator m_it;
      mutable Rect m_rect;
      template<typename> friend class RegionIterator;
      friend class ::gfx::Region;
    };

  } // namespace details

  class Region {
  public:
    enum Overlap { Out, In, Part };

    using iterator = details::RegionIterator<Rect>;
    using const_iterator = details::RegionIterator<const Rect>;

    Region();
    Region(const Region& copy);
    explicit Region(const Rect& rect);
    Region& operator=(const Rect& rect);
    Region& operator=(const Region& copy);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    bool isEmpty() const { return m_region.isEmpty(); }
    bool isRect() const { return m_region.isRect(); }
    bool isComplex() const { return m_region.isComplex(); }

    std::size_t size() const {
      return m_region.computeRegionComplexity();
    }

    Rect bounds() const;

    void clear() {
      m_region.setEmpty();
    }

    void offset(int dx, int dy) {
      m_region.translate(dx, dy);
    }

    void offset(const PointT<int>& delta) {
      m_region.translate(delta.x, delta.y);
    }

    Region& createIntersection(const Region& a, const Region& b) {
      m_region.op(a.m_region, b.m_region, SkRegion::kIntersect_Op);
      return *this;
    }

    Region& createUnion(const Region& a, const Region& b) {
      m_region.op(a.m_region, b.m_region, SkRegion::kUnion_Op);
      return *this;
    }

    Region& createSubtraction(const Region& a, const Region& b) {
      m_region.op(a.m_region, b.m_region, SkRegion::kDifference_Op);
      return *this;
    }

    bool contains(const PointT<int>& pt) const {
      return m_region.contains(pt.x, pt.y);
    }
    Overlap contains(const Rect& rect) const;

    Region& operator+=(const Region& b) { return createUnion(*this, b); }
    Region& operator|=(const Region& b) { return createUnion(*this, b); }
    Region& operator&=(const Region& b) { return createIntersection(*this, b); }
    Region& operator-=(const Region& b) { return createSubtraction(*this, b); }

  private:
    mutable details::Region m_region;
  };

} // namespace gfx

#endif
