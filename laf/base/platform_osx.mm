// LAF Base Library
// Copyright (c) 2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/platform.h"

#include <Cocoa/Cocoa.h>

namespace base {

Version get_osx_version()
{
  int major = 10;
  int minor = 0;
  int patch = 0;

  NSProcessInfo* info = [NSProcessInfo processInfo];
  if ([info respondsToSelector:@selector(operatingSystemVersion)]) {
    NSOperatingSystemVersion osVer = [info operatingSystemVersion];
    major = osVer.majorVersion;
    minor = osVer.minorVersion;
    patch = osVer.patchVersion;
  }
  else {
    SInt32 systemVersion, majorVersion, minorVersion, patchVersion;
    if (Gestalt(gestaltSystemVersion, &systemVersion) != noErr)
      return Version();
    if (systemVersion < 0x1040) {
      major = ((systemVersion & 0xF000) >> 12) * 10 + ((systemVersion & 0x0F00) >> 8);
      minor = (systemVersion & 0x00F0) >> 4;
      patch = (systemVersion & 0x000F);
    }
    else if (Gestalt(gestaltSystemVersionMajor, &majorVersion) == noErr &&
             Gestalt(gestaltSystemVersionMinor, &minorVersion) == noErr &&
             Gestalt(gestaltSystemVersionBugFix, &patchVersion) == noErr) {
      major = majorVersion;
      minor = minorVersion;
      patch = patchVersion;
    }
  }

  return Version(major, minor, patch, 0);
}

} // namespace base
