// LAF Base Library
// Copyright (c) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_WIN_VER_QUERY_VALUES_H_INCLUDED
#define BASE_WIN_VER_QUERY_VALUES_H_INCLUDED
#pragma once

#include "base/dll.h"

#include <map>
#include <string>

namespace base {

std::map<std::string, std::string> ver_query_values(dll lib);
std::map<std::string, std::string> ver_query_values(const wchar_t* filename);

} // namespace base

#endif
