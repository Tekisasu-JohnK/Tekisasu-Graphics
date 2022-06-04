// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_MENUS_H_INCLUDED
#define OS_OSX_MENUS_H_INCLUDED
#pragma once

#include "os/menus.h"

namespace os {

  class MenusOSX : public Menus {
  public:
    MenusOSX();
    MenuRef makeMenu() override;
    MenuItemRef makeMenuItem(const MenuItemInfo& info) override;
    void setAppMenu(const MenuRef& menu) override;
  };

} // namespace os

#endif
