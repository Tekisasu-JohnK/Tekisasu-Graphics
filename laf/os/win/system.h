// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_SYSTEM_H
#define OS_WIN_SYSTEM_H
#pragma once

#include "os/common/system.h"
#include "os/win/winapi.h"
#include "os/win/wintab.h"

namespace os {

class WindowSystem : public CommonSystem {
public:
  WindowSystem();
  ~WindowSystem();

  WinAPI& winApi() { return m_winApi; }
  WintabAPI& wintabApi() { return m_wintabApi; }

  void setAppName(const std::string& appName) override;
  std::string appName() const { return m_appName; }

  void setTabletAPI(TabletAPI api) override;
  TabletAPI tabletAPI() const override { return m_tabletAPI; }

  bool isKeyPressed(KeyScancode scancode) override;
  int getUnicodeFromScancode(KeyScancode scancode) override;

private:
  std::string m_appName;
  TabletAPI m_tabletAPI = TabletAPI::Default;
  WinAPI m_winApi;
  WintabAPI m_wintabApi;
};

} // namespace os

#endif
