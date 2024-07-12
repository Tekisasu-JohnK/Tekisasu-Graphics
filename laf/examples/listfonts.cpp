// LAF Library
// Copyright (c) 2019-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/os.h"

#include <cassert>
#include <cstdio>

static const char* to_str(os::FontStyle::Weight weight)
{
  switch (weight) {
    case os::FontStyle::Weight::Invisible: return "Invisible";
    case os::FontStyle::Weight::Thin: return "Thin";
    case os::FontStyle::Weight::ExtraLight: return "ExtraLight";
    case os::FontStyle::Weight::Light: return "Light";
    case os::FontStyle::Weight::Normal: return "Normal";
    case os::FontStyle::Weight::Medium: return "Medium";
    case os::FontStyle::Weight::SemiBold: return "SemiBold";
    case os::FontStyle::Weight::Bold: return "Bold";
    case os::FontStyle::Weight::ExtraBold: return "ExtraBold";
    case os::FontStyle::Weight::Black: return "Black";
    case os::FontStyle::Weight::ExtraBlack: return "ExtraBlack";
  }
  return "";
}

static const char* to_str(os::FontStyle::Width width)
{
  switch (width) {
    case os::FontStyle::Width::UltraCondensed: return "UltraCondensed";
    case os::FontStyle::Width::ExtraCondensed: return "ExtraCondensed";
    case os::FontStyle::Width::Condensed: return "Condensed";
    case os::FontStyle::Width::SemiCondensed: return "SemiCondensed";
    case os::FontStyle::Width::Normal: return "Normal";
    case os::FontStyle::Width::SemiExpanded: return "SemiExpanded";
    case os::FontStyle::Width::Expanded: return "Expanded";
    case os::FontStyle::Width::ExtraExpanded: return "ExtraExpanded";
    case os::FontStyle::Width::UltraExpanded: return "UltraExpanded";
  }
  return "";
}

static const char* to_str(os::FontStyle::Slant slant)
{
  switch (slant) {
    case os::FontStyle::Slant::Upright: return "Upright";
    case os::FontStyle::Slant::Italic: return "Italic";
    case os::FontStyle::Slant::Oblique: return "Oblique";
  }
  return "";
}

static void print_set(const std::string& name, os::FontStyleSet* set)
{
  for (int j=0; j<set->count(); ++j) {
    os::FontStyle style;
    std::string styleName;
    set->getStyle(j, style, styleName);
    std::printf(" * %s (%s %s %s)\n",
                name.c_str(),
                to_str(style.weight()),
                to_str(style.width()),
                to_str(style.slant()));
  }
}

int app_main(int argc, char* argv[])
{
  os::SystemRef system = os::make_system();
  system->setAppMode(os::AppMode::CLI);

  auto fm = system->fontManager();
  if (!fm) {
    std::printf("There is no font manager in your platform\n");
    return 1;
  }

  if (argc > 1) {
    for (int i=1; i<argc; ++i) {
      std::string name = argv[i];
      std::printf("%s\n", name.c_str());
      auto set = fm->matchFamily(name);
      if (!set) {
        std::printf("Font family '%s' not found\n", argv[i]);
        return 1;
      }
      print_set(name, set.get());
    }
  }
  // Print all font families
  else {
    const int n = fm->countFamilies();
    for (int i=0; i<n; ++i) {
      std::string name = fm->familyName(i);
      std::printf("%s\n", name.c_str());

      auto fnset = fm->matchFamily(name);
      auto set = fm->familyStyleSet(i);
      assert(fnset->count() == set->count());

      print_set(name, set.get());
    }
  }
  return 0;
}
