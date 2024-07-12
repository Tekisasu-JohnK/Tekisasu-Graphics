// LAF Gfx Library
// Copyright (C) 2019-2022  Igara Studio S.A.
// Copyright (C) 2001-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_REGION_PIXMAN_H_INCLUDED
#define GFX_REGION_PIXMAN_H_INCLUDED
#pragma once

#include "gfx/rect.h"

#include <vector>

namespace gfx {

  template<typename T> class PointT;

  class Region;

  namespace details {

  #ifdef PIXMAN_VERSION_MAJOR
    typedef struct pixman_box32 Box;
    typedef struct pixman_region32 Region;
  #else
    struct Box {
      int32_t x1, y1, x2, y2;
    };
    struct Region {
      Box extents;
      void* data;
    };
  #endif

    template<typename T>
    class RegionIterator {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T*;
      using reference = T&;

      RegionIterator() : m_ptr(nullptr) { }
      RegionIterator(const RegionIterator& o) : m_ptr(o.m_ptr) { }
      template<typename T2>
      RegionIterator(const RegionIterator<T2>& o) : m_ptr(o.m_ptr) { }
      RegionIterator& operator=(const RegionIterator& o) { m_ptr = o.m_ptr; return *this; }
      RegionIterator& operator++() { ++m_ptr; return *this; }
      RegionIterator operator++(int) { RegionIterator o(*this); ++m_ptr; return o; }
      bool operator==(const RegionIterator& o) const { return m_ptr == o.m_ptr; }
      bool operator!=(const RegionIterator& o) const { return m_ptr != o.m_ptr; }
      reference operator*() {
        m_rect.x = m_ptr->x1;
        m_rect.y = m_ptr->y1;
        m_rect.w = m_ptr->x2 - m_ptr->x1;
        m_rect.h = m_ptr->y2 - m_ptr->y1;
        return m_rect;
      }
    private:
      Box* m_ptr;
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
    ~Region();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    bool isEmpty() const;
    bool isRect() const;
    bool isComplex() const;
    std::size_t size() const;
    Rect bounds() const;

    void clear();

    void offset(int dx, int dy);
    void offset(const PointT<int>& delta);

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
    mutable details::Region m_region;
  };

} // namespace gfx

#endif
