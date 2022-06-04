// LAF OS Library
// Copyright (C) 2018-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_COLOR_SPACE_H_INCLUDED
#define OS_OSX_COLOR_SPACE_H_INCLUDED
#pragma once

#include "os/color_space.h"

#ifdef __OBJC__
#include <Cocoa/Cocoa.h>
#endif

#include <vector>

namespace os {

#ifdef __OBJC__
os::ColorSpaceRef convert_nscolorspace_to_os_colorspace(NSColorSpace* nsColorSpace);
#endif

void list_display_colorspaces(std::vector<os::ColorSpaceRef>& list);

} // namespace os

#endif
