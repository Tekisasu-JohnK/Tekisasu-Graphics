// LAF Gfx Library
// Copyright (C) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_REGION_WIN_H_INCLUDED
#define GFX_REGION_WIN_H_INCLUDED
#pragma once

#include "gfx/point.h"
#include "gfx/rect.h"

#include <vector>

#include <windows.h>

namespace gfx {

  template<typename T> class PointT;

  class Region;

  namespace details {

    template<typename T>
    class RegionIterator {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T*;
      using reference = T&;

      RegionIterator() { }
      RegionIterator(LPRECT prect) : m_prect(prect) { }
      RegionIterator(const RegionIterator& o) : m_prect(o.m_prect) { }
      template<typename T2>
      RegionIterator(const RegionIterator<T2>& o) : m_prect(o.m_prect) { }
      RegionIterator& operator=(const RegionIterator& o) { m_prect = o.m_prect; return *this; }
      RegionIterator& operator++() { ++m_prect; return *this; }
      bool operator==(const RegionIterator& o) const {
        return (m_prect == o.m_prect);
      }
      bool operator!=(const RegionIterator& o) const {
        return (m_prect != o.m_prect);
      }
      reference operator*() {
        m_rect.x = m_prect->left;
        m_rect.y = m_prect->top;
        m_rect.w = m_prect->right - m_prect->left;
        m_rect.h = m_prect->bottom - m_prect->top;
        return m_rect;
      }
    private:
      LPRECT m_prect;
      gfx::Rect m_rect;
      template<typename> friend class RegionIterator;
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
    ~Region();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    bool isEmpty() const { RECT rc; return GetRgnBox(m_hrgn, &rc) == NULLREGION; }
    bool isRect() const { RECT rc; return GetRgnBox(m_hrgn, &rc) == SIMPLEREGION; }
    bool isComplex() const { RECT rc; return GetRgnBox(m_hrgn, &rc) == COMPLEXREGION; }

    std::size_t size() const;

    Rect bounds() const;

    void clear();

    void offset(int dx, int dy);
    void offset(const PointT<int>& delta) {
      offset(delta.x, delta.y);
    }

    Region& createIntersection(const Region& a, const Region& b);
    Region& createUnion(const Region& a, const Region& b);
    Region& createSubtraction(const Region& a, const Region& b);

    bool contains(const PointT<int>& pt) const;
    Overlap contains(const Rect& rect) const;

    Region& operator+=(const Region& b) { return createUnion(*this, b); }
    Region& operator|=(const Region& b) { return createUnion(*this, b); }
    Region& operator&=(const Region& b) { return createIntersection(*this, b); }
    Region& operator-=(const Region& b) { return createSubtraction(*this, b); }

  private:
    void resetData() const;
    void fillData() const;

    HRGN m_hrgn = nullptr;
    mutable LPRGNDATA m_data = nullptr;
  };

} // namespace gfx

#endif
