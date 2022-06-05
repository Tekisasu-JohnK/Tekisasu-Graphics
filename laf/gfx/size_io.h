// LAF Gfx Library
// Copyright (C) 2019-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_SIZE_IO_H_INCLUDED
#define GFX_SIZE_IO_H_INCLUDED
#pragma once

#include "gfx/size.h"
#include <iostream>

namespace gfx {

  template<typename T>
  inline std::ostream& operator<<(std::ostream& os, const SizeT<T>& size) {
    return os << "("
              << size.w << ", "
              << size.h << ")";
  }

  template<typename T>
  inline std::istream& operator>>(std::istream& in, SizeT<T>& size) {
    while (in && in.get() != '(')
      ;

    if (!in)
      return in;

    char chr;
    in >> size.w >> chr
       >> size.h;

    return in;
  }

}

#endif
