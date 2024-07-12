// LAF OS Library
// Copyright (C) 2019-2023  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COMMON_SYSTEM_H
#define OS_COMMON_SYSTEM_H
#pragma once

#if LAF_WINDOWS
  #include "os/win/native_dialogs.h"
#elif LAF_MACOS
  #include "os/osx/app.h"
  #include "os/osx/menus.h"
  #include "os/osx/native_dialogs.h"
#elif LAF_LINUX
  #include "os/x11/native_dialogs.h"
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
  ~CommonSystem() {
    // destroyInstance() can be called multiple times by derived
    // classes.
    if (instance() == this)
      destroyInstance();
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
    if (!m_nativeDialogs) {
#if LAF_WINDOWS
      m_nativeDialogs = Ref<NativeDialogs>(new NativeDialogsWin);
#elif LAF_MACOS
      m_nativeDialogs = Ref<NativeDialogs>(new NativeDialogsOSX);
#elif LAF_LINUX
      m_nativeDialogs = Ref<NativeDialogs>(new NativeDialogsX11);
#endif
    }
    return m_nativeDialogs.get();
  }

  EventQueue* eventQueue() override {
    return EventQueue::instance();
  }

  FontRef loadSpriteSheetFont(const char* filename, int scale) override {
    SurfaceRef sheet = loadRgbaSurface(filename);
    FontRef font = nullptr;
    if (sheet) {
      sheet->applyScale(scale);
      sheet->setImmutable();
      font = SpriteSheetFont::fromSurface(sheet);
    }
    return font;
  }

  FontRef loadTrueTypeFont(const char* filename, int height) override {
#ifdef LAF_FREETYPE
    if (!m_ft)
      m_ft.reset(new ft::Lib());
    return FontRef(load_free_type_font(*m_ft.get(), filename, height));
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

protected:
  // This must be called in the final class that derived from
  // CommonSystem, because clearing the list of events can generate
  // events on windows that will depend on the platform-specific
  // System.
  //
  // E.g. We've crash reports because WindowWin is receiving
  // WM_ACTIVATE messages when we destroy the events queue, and the
  // handler of that message is expecting the SystemWin instance (not
  // a CommonSystem instance). That's why we cannot call this from
  // ~CommonSystem() destructor and we have to call this from
  // ~SystemWin (or other platform-specific System implementations).
  void destroyInstance() {
    // We have to reset the list of all events to clear all possible
    // living WindowRef (so all window destructors are called at this
    // point, when the os::System instance is still alive).
    //
    // TODO Maybe the event queue should be inside the System instance
    //      (so when the system is deleted, the queue is
    //      deleted). Anyway we should still clear all the events
    //      before set_instance(nullptr), and we're not sure if this
    //      is possible on macOS, as some events are queued before the
    //      System instance is even created (see
    //      EventQueue::instance() comment on laf/os/event_queue.h).
    eventQueue()->clearEvents();

    set_instance(nullptr);
  }

private:
  Ref<NativeDialogs> m_nativeDialogs;
#ifdef LAF_FREETYPE
  std::unique_ptr<ft::Lib> m_ft;
#endif
};

} // namespace os

#endif
