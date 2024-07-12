// LAF Gfx Library
// Copyright (C) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_BORDER_IO_H_INCLUDED
#define GFX_BORDER_IO_H_INCLUDED
#pragma once

#include "gfx/border.h"
#include <iostream>

namespace gfx {

  template<typename T>
  inline std::ostream& operator<<(std::ostream& os, const BorderT<T>& border) {
    return os << "("
              << border.left() << ", "
              << border.top() << ", "
              << border.right() << ", "
              << border.bottom() << ")";
  }

  template<typename T>
  inline std::istream& operator>>(std::istream& in, BorderT<T>& border) {
    while (in && in.get() != '(')
      ;

    if (!in)
      return in;

    T l, t, r, b;
    char chr;
    in >> l >> chr
       >> t >> chr
       >> r >> chr
       >> b >> chr;

    border.left(l);
    border.top(t);
    border.right(r);
    border.bottom(b);

    return in;
  }

}

#endif
