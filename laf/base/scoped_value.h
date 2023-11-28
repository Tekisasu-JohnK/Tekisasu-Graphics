// LAF Base Library
// Copyright (c) 2022-2023 Igara Studio S.A.
// Copyright (c) 2014-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_SCOPED_VALUE_H_INCLUDED
#define BASE_SCOPED_VALUE_H_INCLUDED
#pragma once

namespace base {

  // Changes the value of "instance" with "inValue" in the
  // ScopedValue's lifespan, then restore its value (or restore
  // another value as specified by "outValue").
  template<typename T,
           typename U = T>
  class ScopedValue {
  public:
    ScopedValue(T& instance, const U inValue)
      : m_instance(instance)
      , m_outValue(instance) { // Restore the current value
      m_instance = inValue;
    }

    ScopedValue(T& instance, const U inValue, const U outValue)
      : m_instance(instance)
      , m_outValue(outValue) {
      m_instance = inValue;
    }

    ~ScopedValue() {
      m_instance = m_outValue;
    }

  private:
    T& m_instance;
    U m_outValue;
  };

} // namespace base

#endif
