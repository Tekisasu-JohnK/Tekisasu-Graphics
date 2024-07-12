// LAF OS Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_REF_H_INCLUDED
#define OS_REF_H_INCLUDED
#pragma once

#include "base/ref.h"

namespace os {

  template<typename T>
  using RefCountT = base::RefCountT<T>;
  using RefCount = base::RefCount;

  template<typename T>
  using Ref = base::Ref<T>;

  template<typename T,
           typename ...Args>
  Ref<T> make_ref(Args&&...args) {
    return base::make_ref<T>(std::forward<Args>(args)...);
  }

} // namespace os

#endif
