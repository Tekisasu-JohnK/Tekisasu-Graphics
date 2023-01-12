// LAF OS Library
// Copyright (C) 2020-2022  Igara Studio S.A.
// Copyright (C) 2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_MENUS_H_INCLUDED
#define OS_MENUS_H_INCLUDED
#pragma once

#include "os/keys.h"
#include "os/ref.h"
#include "os/shortcut.h"

#include <functional>
#include <string>

namespace os {
  class MenuItem;

  struct MenuItemInfo {
    enum Type {
      Normal,
      Separator
    };

    enum Action {
      UserDefined,

      // macOS standard commands
      Hide,
      HideOthers,
      ShowAll,
      Quit,
      Minimize,
      Zoom,
    };

    Type type;
    Action action;
    std::string text;
    Shortcut shortcut;
    std::function<void()> execute;
    std::function<void(os::MenuItem*)> validate;

    explicit MenuItemInfo(const Type type = Normal,
                          const Action action = UserDefined)
      : type(type)
      , action(action) {
    }

    explicit MenuItemInfo(const std::string& text,
                          const Action action = UserDefined)
      : type(Normal)
      , action(action)
      , text(text) {
    }
  };

  class Menu;
  class MenuItem;
  class Menus;

  using MenuItemRef = Ref<MenuItem>;
  using MenuRef = Ref<Menu>;
  using MenusRef = Ref<Menus>;

  class MenuItem : public RefCount {
  public:
    virtual ~MenuItem() { }
    virtual void setText(const std::string& text) = 0;
    virtual void setSubmenu(const MenuRef& submenu) = 0;
    virtual void setEnabled(bool state) = 0;
    virtual void setChecked(bool state) = 0;
    virtual void setShortcut(const Shortcut& shortcut) = 0;

    // Indicates that this is the "Edit" menu with the
    // Cut/Copy/Paste/etc. commands. Used by the laf implementation on
    // macOS to switch the "Edit" menu completely with a standard
    // implementation when necessary.
    //
    // If you are going to use a native file dialog (os::FileDialog),
    // you will need to specify a standard "Edit" menu to make the
    // Undo/Redo/Cut/Copy/Paste/etc. keyboard shortcuts work.
    //
    // The specific details are not beautiful or important, but you
    // can search for "OSXEditMenuHack" to know why this is needed.
    virtual void setAsStandardEditMenuItem() = 0;
  };

  class Menu : public RefCount {
  public:
    virtual ~Menu() { }
    virtual void addItem(const MenuItemRef& item) = 0;
    virtual void insertItem(const int index, const MenuItemRef& item) = 0;
    virtual void removeItem(const MenuItemRef& item) = 0;
  };

  class Menus : public RefCount {
  public:
    virtual ~Menus() { }
    virtual MenuRef makeMenu() = 0;
    virtual MenuItemRef makeMenuItem(const MenuItemInfo& info) = 0;
    virtual void setAppMenu(const MenuRef& menu) = 0;
  };

} // namespace os

#endif
