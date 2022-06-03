// LAF OS Library
// Copyright (C) 2018  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/osx/color_space.h"

#include "os/system.h"

namespace os {

os::ColorSpacePtr convert_nscolorspace_to_os_colorspace(NSColorSpace* nsColorSpace)
{
  os::ColorSpacePtr osCS;
  CGColorSpaceRef cgCS = [nsColorSpace CGColorSpace];
  if (cgCS) {
    CFDataRef icc = CGColorSpaceCopyICCProfile(cgCS);
    if (icc) {
      auto gfxCS = gfx::ColorSpace::MakeICC(CFDataGetBytePtr(icc),
                                            CFDataGetLength(icc));

      gfxCS->setName(
        std::string("Display Profile: ") +
        [[nsColorSpace localizedName] UTF8String]);

      osCS = os::instance()->createColorSpace(gfxCS);
      CFRelease(icc);
    }
  }
  return osCS;
}

void list_display_colorspaces(std::vector<os::ColorSpacePtr>& list)
{
  // One color profile for each screen
  for (NSScreen* screen in [NSScreen screens]) {
    os::ColorSpacePtr osCS =
      convert_nscolorspace_to_os_colorspace([screen colorSpace]);
    if (osCS)
      list.push_back(osCS);
  }
}

} // namespace os
