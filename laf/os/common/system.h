// LAF OS Library
// Copyright (C) 2019-2024  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COMMON_SYSTEM_H
#define OS_COMMON_SYSTEM_H
#pragma once

#ifdef LAF_FREETYPE
#include "ft/lib.h"
#include "os/common/freetype_font.h"
#endif
#include "os/common/sprite_sheet_font.h"
#include "os/event_queue.h"
#include "os/menus.h"
#include "os/system.h"

#if CLIP_ENABLE_IMAGE
#include "clip/clip.h"
#endif

namespace os {

class CommonSystem : public System {
public:
  CommonSystem();
  ~CommonSystem();

  const std::string& appName() const override { return m_appName; }
  void setAppName(const std::string& appName) override { m_appName = appName; }
  void setAppMode(AppMode appMode) override { }

  void markCliFileAsProcessed(const std::string& fn) override { }
  void finishLaunching() override { }
  void activateApp() override { }

  // Do nothing options (these functions are for Windows-only at the
  // moment)
  void setTabletOptions(const TabletOptions&) override { }
  TabletOptions tabletOptions() const override { return TabletOptions(); }

  Logger* logger() override {
    return nullptr;
  }

  Menus* menus() override {
    return nullptr;
  }

  NativeDialogs* nativeDialogs() override;

  EventQueue* eventQueue() override {
    return EventQueue::instance();
  }

  FontRef loadSpriteSheetFont(const char* filename, int scale) override;
  FontRef loadTrueTypeFont(const char* filename, int height) override;

  KeyModifiers keyModifiers() override;

#if CLIP_ENABLE_IMAGE
  SurfaceRef makeSurface(const clip::image& image) override;
#endif

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
  void destroyInstance();

private:
  std::string m_appName;
  Ref<NativeDialogs> m_nativeDialogs;
#ifdef LAF_FREETYPE
  std::unique_ptr<ft::Lib> m_ft;
#endif
};

} // namespace os

#endif
