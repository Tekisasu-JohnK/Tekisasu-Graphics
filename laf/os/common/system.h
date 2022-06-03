// LAF OS Library
// Copyright (C) 2019-2020  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COMMON_SYSTEM_H
#define OS_COMMON_SYSTEM_H
#pragma once

#ifdef _WIN32
  #include "os/win/native_dialogs.h"
#elif defined(__APPLE__)
  #include "os/osx/app.h"
  #include "os/osx/menus.h"
  #include "os/osx/native_dialogs.h"
#elif defined(LAF_OS_WITH_GTK)
  #include "os/gtk/native_dialogs.h"
#else
  #include "os/native_dialogs.h"
#endif

#ifdef LAF_FREETYPE
#include "ft/lib.h"
#include "os/common/freetype_font.h"
#endif
#include "os/common/sprite_sheet_font.h"
#include "os/event_queue.h"
#include "os/menus.h"
#include "os/system.h"

#include <memory>

namespace os {

class CommonSystem : public System {
public:
  CommonSystem() { }
  ~CommonSystem() { }

  void dispose() override {
    set_instance(nullptr);
    delete this;
  }

  void setAppName(const std::string& appName) override { }
  void setAppMode(AppMode appMode) override { }

  void markCliFileAsProcessed(const std::string& fn) override { }
  void finishLaunching() override { }
  void activateApp() override { }

  void setTabletAPI(TabletAPI api) override {
    // Do nothing by default
  }

  TabletAPI tabletAPI() const override {
    return TabletAPI::Default;
  }

  Logger* logger() override {
    return nullptr;
  }

  Menus* menus() override {
    return nullptr;
  }

  NativeDialogs* nativeDialogs() override {
#ifdef _WIN32
    if (!m_nativeDialogs)
      m_nativeDialogs.reset(new NativeDialogsWin32);
#elif defined(__APPLE__)
    if (!m_nativeDialogs)
      m_nativeDialogs.reset(new NativeDialogsOSX);
#elif defined(LAF_OS_WITH_GTK)
    if (!m_nativeDialogs)
      m_nativeDialogs.reset(new NativeDialogsGTK);
#endif
    return m_nativeDialogs.get();
  }

  EventQueue* eventQueue() override {
    return EventQueue::instance();
  }

  Font* loadSpriteSheetFont(const char* filename, int scale) override {
    Surface* sheet = loadRgbaSurface(filename);
    Font* font = nullptr;
    if (sheet) {
      sheet->applyScale(scale);
      font = SpriteSheetFont::fromSurface(sheet);
    }
    return font;
  }

  Font* loadTrueTypeFont(const char* filename, int height) override {
#ifdef LAF_FREETYPE
    if (!m_ft)
      m_ft.reset(new ft::Lib());
    return load_free_type_font(*m_ft.get(), filename, height);
#else
    return nullptr;
#endif
  }

  KeyModifiers keyModifiers() override {
    return
      (KeyModifiers)
      ((isKeyPressed(kKeyLShift) ||
        isKeyPressed(kKeyRShift) ? kKeyShiftModifier: 0) |
       (isKeyPressed(kKeyLControl) ||
        isKeyPressed(kKeyRControl) ? kKeyCtrlModifier: 0) |
       (isKeyPressed(kKeyAlt) ? kKeyAltModifier: 0) |
       (isKeyPressed(kKeyAltGr) ? (kKeyCtrlModifier | kKeyAltModifier): 0) |
       (isKeyPressed(kKeyCommand) ? kKeyCmdModifier: 0) |
       (isKeyPressed(kKeySpace) ? kKeySpaceModifier: 0) |
       (isKeyPressed(kKeyLWin) ||
        isKeyPressed(kKeyRWin) ? kKeyWinModifier: 0));
  }

private:
  std::unique_ptr<NativeDialogs> m_nativeDialogs;
#ifdef LAF_FREETYPE
  std::unique_ptr<ft::Lib> m_ft;
#endif
};

} // namespace os

#endif
