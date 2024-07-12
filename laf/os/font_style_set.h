// LAF OS Library
// Copyright (c) 2019-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_FONT_STYLE_SET_H_INCLUDED
#define OS_FONT_STYLE_SET_H_INCLUDED
#pragma once

#include "os/ref.h"
#include "os/typeface.h"

namespace os {

  class FontStyle;

  class FontStyleSet : public RefCount {
  protected:
    virtual ~FontStyleSet() { }
  public:
    virtual int count() = 0;
    virtual void getStyle(int index,
                          FontStyle& style,
                          std::string& name) = 0;
    virtual Ref<Typeface> typeface(int index) = 0;
    virtual Ref<Typeface> matchStyle(const FontStyle& style) = 0;
  };

} // namespace os

#endif
