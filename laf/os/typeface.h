// LAF OS Library
// Copyright (c) 2019-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_TYPEFACE_H_INCLUDED
#define OS_TYPEFACE_H_INCLUDED
#pragma once

#include "os/ref.h"

namespace os {

  class Typeface;
  using TypefaceRef = base::Ref<Typeface>;

  class Typeface : public RefCount {
  protected:
    virtual ~Typeface() { }
  public:
    virtual FontStyle fontStyle() const = 0;
  };

} // namespace os

#endif
