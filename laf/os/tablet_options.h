// LAF OS Library
// Copyright (c) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_TABLET_OPTIONS_H_INCLUDED
#define OS_TABLET_OPTIONS_H_INCLUDED
#pragma once

namespace os {

  // Windows-specific details: API to use to get tablet input
  // information.
  enum class TabletAPI {
    // Default tablet API to use in the system (Windows Ink on
    // Windows; only valid value on other systems).
    Default = 0,

    // Use Windows 8/10 pointer messages (Windows Ink).
    WindowsPointerInput = 0,

    // Use the Wintab API to get pressure information from packets but
    // mouse movement from Windows system messages
    // (WM_MOUSEMOVE).
    Wintab = 1,

    // Use the Wintab API processing packets directly (pressure and
    // stylus movement information). With this we might get more
    // precision from the device (but still work-in-progress, some
    // messages might be mixed up).
    WintabPackets = 2,
  };

  struct TabletOptions {
#if LAF_WINDOWS
    // Windows API to get stylus/digital tablet position of the pen
    // (Windows Pointer API, legacy Wintab, etc.).
    TabletAPI api = TabletAPI::Default;

    // Use a fix for live streaming software like OBS to set the
    // cursor position each time we receive a pointer event like
    bool setCursorFix = false;
#endif

#if LAF_LINUX
    // An user-defined string to detect the stylus device from
    // "xinput --list" command.
    std::string detectStylusPattern;
#endif
  };

} // namespace os

#endif
