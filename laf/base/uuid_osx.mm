// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/uuid.h"

#include <Foundation/Foundation.h>

namespace base {

Uuid Uuid::Generate()
{
  Uuid uuid;
  NSUUID* nsUuid = [NSUUID UUID];
  if (nsUuid) {
    [nsUuid getUUIDBytes:(unsigned char*)uuid.bytes()];

#if LAF_BASE_TRACE_UUID
    const char* str = nsUuid.UUIDString.UTF8String;
    printf("convert_to = \"%s\"\n"
           "NSUUID     = \"%s\"\n",
           base::convert_to<std::string>(uuid).c_str(),
           base::string_to_lower(std::string(str)).c_str());
#endif
  }
  return uuid;
}

} // namespace base
