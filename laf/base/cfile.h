// LAF Base Library
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_CFILE_H_INCLUDED
#define BASE_CFILE_H_INCLUDED
#pragma once

#include <cstdio>

namespace base {

  int fgetw(FILE* file);
  long fgetl(FILE* file);
  long long fgetq(FILE* file);
  float fgetf(FILE* file);
  double fgetd(FILE* file);
  int fputw(int w, FILE* file);
  int fputl(long l, FILE* file);
  int fputq(long long l, FILE* file);
  int fputf(float l, FILE* file);
  int fputd(double l, FILE* file);

} // namespace base

#endif
