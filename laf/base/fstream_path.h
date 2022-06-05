// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_FSTREAM_PATH_H_INCLUDED
#define BASE_FSTREAM_PATH_H_INCLUDED
#pragma once

#include "base/string.h"

#if LAF_WINDOWS
  #ifdef __MINGW32__
    #define FSTREAM_PATH(path) (std::string(path).c_str())
  #else
    #define FSTREAM_PATH(path) (base::from_utf8(path).c_str())
  #endif
#else
  #define FSTREAM_PATH(path) (std::string(path).c_str())
#endif

#endif
