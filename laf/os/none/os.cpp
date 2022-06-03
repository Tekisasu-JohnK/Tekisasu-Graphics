// LAF OS Library
// Copyright (c) 2018-2020  Igara Studio S.A.
// Copyright (c) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/memory.h"
#include "base/string.h"
#include "gfx/size.h"
#include "os/system.h"

namespace os {

class NoneSystem : public System {
public:
  void dispose() override { delete this; }
  void setAppName(const std::string& appName) override { }
  void setAppMode(AppMode appMode) override { }

  void markCliFileAsProcessed(const std::string& fn) override { }
  void finishLaunching() override { }
  void activateApp() override { }

  Capabilities capabilities() const override { return (Capabilities)0; }
  void setTabletAPI(TabletAPI api) override { }
  TabletAPI tabletAPI() const override { return TabletAPI::Default; }
  Logger* logger() override { return nullptr; }
  Menus* menus() override { return nullptr; }
  NativeDialogs* nativeDialogs() override { return nullptr; }
  EventQueue* eventQueue() override { return nullptr; }
  bool gpuAcceleration() const override { return false; }
  void setGpuAcceleration(bool state) override { }
  gfx::Size defaultNewDisplaySize() override { return gfx::Size(0, 0); }
  Display* defaultDisplay() override { return nullptr; }
  Display* createDisplay(int width, int height, int scale) override { return nullptr; }
  Surface* createSurface(int width, int height,
                         const os::ColorSpacePtr& colorSpace) override { return nullptr; }
  Surface* createRgbaSurface(int width, int height,
                             const os::ColorSpacePtr& colorSpace) override { return nullptr; }
  Surface* loadSurface(const char* filename) override { return nullptr; }
  Surface* loadRgbaSurface(const char* filename) override { return nullptr; }
  FontManager* fontManager() { return nullptr; }
  Font* loadSpriteSheetFont(const char* filename, int scale) override { return nullptr; }
  Font* loadTrueTypeFont(const char* filename, int height) override { return nullptr; }
  bool isKeyPressed(KeyScancode scancode) override { return false; }
  KeyModifiers keyModifiers() override { return kKeyNoneModifier; }
  int getUnicodeFromScancode(KeyScancode scancode) override { return 0; }
  void setTranslateDeadKeys(bool state) override { }
  void listColorSpaces(std::vector<os::ColorSpacePtr>& list) override { }
  os::ColorSpacePtr createColorSpace(const gfx::ColorSpacePtr& cs) override { return nullptr; }
  std::unique_ptr<ColorSpaceConversion> convertBetweenColorSpace(
    const os::ColorSpacePtr& src, const os::ColorSpacePtr& dst) override { return nullptr; }
  void setDisplaysColorSpace(const os::ColorSpacePtr& cs) override { }
  os::ColorSpacePtr displaysColorSpace() override { return nullptr; }
};

System* create_system_impl() {
  return new NoneSystem;
}

void error_message(const char* msg)
{
  fputs(msg, stderr);
  // TODO
}

} // namespace os
