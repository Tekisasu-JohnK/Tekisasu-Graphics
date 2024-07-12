// LAF OS Library
// Copyright (C) 2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_CURSOR_H_INCLUDED
#define OS_CURSOR_H_INCLUDED
#pragma once

#include "os/ref.h"

namespace os {
  class Cursor;
  using CursorRef = Ref<Cursor>;

  class Cursor : public RefCount {
  public:
    virtual ~Cursor() { }
    virtual void* nativeHandle() = 0;
  };

} // namespace os

#endif
