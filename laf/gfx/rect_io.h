// LAF Gfx Library
// Copyright (C) 2020-2021  Igara Studio S.A.
// Copyright (C) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_RECT_IO_H_INCLUDED
#define GFX_RECT_IO_H_INCLUDED
#pragma once

#include "gfx/rect.h"
#include <iostream>

namespace gfx {

  template<typename T>
  inline std::ostream& operator<<(std::ostream& os, const RectT<T>& rect) {
    return os << "("
              << rect.x << ", "
              << rect.y << ", "
              << rect.w << ", "
              << rect.h << ")";
  }

  template<typename T>
  inline std::istream& operator>>(std::istream& in, RectT<T>& rect) {
    while (in && in.get() != '(')
      ;

    if (!in)
      return in;

    char chr;
    in >> rect.x >> chr
       >> rect.y >> chr
       >> rect.w >> chr
       >> rect.h >> chr;

    return in;
  }

}

#endif
