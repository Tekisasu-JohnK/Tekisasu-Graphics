// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/convert_to.h"
#include "base/file_content.h"
#include "base/uuid.h"

#include <cstring>

namespace base {

Uuid Uuid::Generate()
{
  Uuid uuid;
  buffer buf = read_file_content("/proc/sys/kernel/random/uuid");
  if (buf.size() >= 16) {
    uuid = base::convert_to<Uuid>(std::string((const char*)&buf[0]));

#if LAF_BASE_TRACE_UUID
    if (buf[buf.size()-1] == '\n')
      buf[buf.size()-1] = 0;
    printf("convert_to  = \"%s\"\n"
           "random/uuid = \"%s\"\n",
           base::convert_to<std::string>(uuid).c_str(), &buf[0]);
#endif
  }
  return uuid;
}

} // namespace base
