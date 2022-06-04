// LAF Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/os.h"

#include <cstdio>

int app_main(int argc, char* argv[])
{
  os::SystemRef system = os::make_system();
  system->setAppMode(os::AppMode::CLI);

  os::ScreenList screens;
  system->listScreens(screens);

  int i = 0;
  for (auto screen : screens) {
    auto rc = screen->bounds();
    auto wa = screen->workarea();
    std::printf("%sscreen=%d bounds=(%d %d %d %d) workarea=(%d %d %d %d)\n",
                screen->isMainScreen() ? "main ": "",
                i++, rc.x, rc.y, rc.w, rc.h,
                wa.x, wa.y, wa.w, wa.h);
  }
  return 0;
}
