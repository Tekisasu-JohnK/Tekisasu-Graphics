// LAF OS Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_TYPEFACE_H_INCLUDED
#define OS_TYPEFACE_H_INCLUDED
#pragma once

#include "os/scoped_handle.h"

namespace os {

  class Typeface {
  protected:
    virtual ~Typeface() { }
  public:
    virtual void dispose() = 0;
    virtual FontStyle fontStyle() const = 0;
  };

  typedef ScopedHandle<Typeface> TypefaceHandle;

} // namespace os

#endif
