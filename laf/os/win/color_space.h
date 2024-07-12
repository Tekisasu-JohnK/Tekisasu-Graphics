// LAF OS Library
// Copyright (C) 2018-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_COLOR_SPACE_H_INCLUDED
#define OS_WIN_COLOR_SPACE_H_INCLUDED
#pragma once

#include "os/color_space.h"

#include <vector>

namespace os {

std::string get_hmonitor_icc_filename(HMONITOR monitor);
os::ColorSpaceRef get_colorspace_from_icc_file(const std::string& iccFilename);
os::ColorSpaceRef get_hmonitor_colorspace(HMONITOR monitor);
void list_display_colorspaces(std::vector<os::ColorSpaceRef>& list);

} // namespace os

#endif
