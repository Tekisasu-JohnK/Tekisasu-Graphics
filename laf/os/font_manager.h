// LAF OS Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_FONT_MANAGER_H_INCLUDED
#define OS_FONT_MANAGER_H_INCLUDED
#pragma once

#include "os/font_style_set.h"
#include "os/scoped_handle.h"

#include <string>

namespace os {

  class FontManager {
  public:
    virtual int countFamilies() const = 0;
    virtual std::string familyName(int index) const = 0;
    virtual FontStyleSetHandle familyStyleSet(int index) const = 0;
    virtual FontStyleSetHandle matchFamily(const std::string& familyName) const = 0;
  protected:
    virtual ~FontManager() { }
  };

} // namespace os

#endif
