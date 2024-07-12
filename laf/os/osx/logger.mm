// LAF OS Library
// Copyright (C) 2021  Igara Studio S.A.
// Copyright (C) 2012-2014  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/osx/logger.h"

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

namespace os {

void LoggerOSX::logError(const char* error)
{
  NSLog([NSString stringWithUTF8String:error]);
}

} // namespace os
