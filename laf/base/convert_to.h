// LAF Base Library
// Copyright (c) 2023 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_CONVERT_TO_H_INCLUDED
#define BASE_CONVERT_TO_H_INCLUDED
#pragma once

#include "base/base.h"
#include "base/ints.h"
#include <string>

namespace base {

  class Sha1;
  class Uuid;

  // Undefined convertion
  template<typename To, typename From>
  To convert_to(const From&) {
    // We've to use the sizeof(To) trick to use static_assert(false)
    // and make the compiler fail in non-specialized cases, see P2593R0:
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2593r0.html
    static_assert(false && sizeof(To), "Invalid conversion"); // NOLINT readability-simplify-boolean-expr
  }

  template<> int convert_to(const std::string& from);
  template<> std::string convert_to(const int& from);

  template<> uint32_t convert_to(const std::string& from);
  template<> std::string convert_to(const uint32_t& from);

  template<> double convert_to(const std::string& from);
  template<> std::string convert_to(const double& from);

  template<> Sha1 convert_to(const std::string& from);
  template<> std::string convert_to(const Sha1& from);

  template<> Uuid convert_to(const std::string& from);
  template<> std::string convert_to(const Uuid& from);

}

#endif
