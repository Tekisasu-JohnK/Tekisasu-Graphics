// LAF OS Library
// Copyright (C) 2021  Igara Studio S.A.
// Copyright (C) 2012-2014  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_LOGGER_H_INCLUDED
#define OS_OSX_LOGGER_H_INCLUDED
#pragma once

#include "os/logger.h"

namespace os {

  class LoggerOSX : public Logger {
  public:
    void logError(const char* error) override;
  };

} // namespace os

#endif // OS_OSX_LOGGER_H_INCLUDED
