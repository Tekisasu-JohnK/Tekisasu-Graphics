// LAF Base Library
// Copyright (c) 2019  Igara Studio S.A.
// Copyright (c) 2015-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_PI_H_INCLUDED
#define BASE_PI_H_INCLUDED
#pragma once

#ifndef PI
#define PI            3.14159265358979323846
#endif

#include <cmath>

namespace base {

// Puts the angle in the -PI to PI range.
inline double fmod_radians(double angle) {
  if (angle < -PI) {
    if (angle < -2.0*PI)
      angle = -std::fmod(-angle, 2.0*PI);
    angle += 2.0*PI;
  }
  if (angle > 2.0*PI)
    angle = std::fmod(angle, 2.0*PI);
  if (angle > PI)
    angle -= 2.0*PI;
  return angle;
}

}

#endif
