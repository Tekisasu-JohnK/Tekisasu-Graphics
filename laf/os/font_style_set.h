// LAF OS Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_FONT_STYLE_SET_H_INCLUDED
#define OS_FONT_STYLE_SET_H_INCLUDED
#pragma once

#include "os/scoped_handle.h"
#include "os/typeface.h"

namespace os {

  class FontStyle;

  class FontStyleSet {
  public:
    virtual void dispose() = 0;
    virtual int count() = 0;
    virtual void getStyle(int index,
                          FontStyle& style,
                          std::string& name) = 0;
    virtual TypefaceHandle typeface(int index) = 0;
    virtual TypefaceHandle matchStyle(const FontStyle& style) = 0;
  protected:
    virtual ~FontStyleSet() { }
  };

  typedef ScopedHandle<FontStyleSet> FontStyleSetHandle;

} // namespace os

#endif
