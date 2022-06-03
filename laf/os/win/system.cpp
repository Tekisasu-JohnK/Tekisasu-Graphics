// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/win/system.h"

namespace os {

bool win_is_key_pressed(KeyScancode scancode);
int win_get_unicode_from_scancode(KeyScancode scancode);

WindowSystem::WindowSystem() { }
WindowSystem::~WindowSystem() { }

void WindowSystem::setAppName(const std::string& appName)
{
  m_appName = appName;
}

void WindowSystem::setTabletAPI(TabletAPI api)
{
  m_tabletAPI = api;
}

bool WindowSystem::isKeyPressed(KeyScancode scancode)
{
  return win_is_key_pressed(scancode);
}

int WindowSystem::getUnicodeFromScancode(KeyScancode scancode)
{
  return win_get_unicode_from_scancode(scancode);
}

} // namespace os
