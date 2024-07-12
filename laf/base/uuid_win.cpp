// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/uuid.h"

#include <cstring>

#include <windows.h>

namespace base {

Uuid Uuid::Generate()
{
  Uuid uuid;

  // We don't use UuidCreate() to avoid depending on rpcrt4 library.
  // Anyway it's said in Windows docs that CoCreateGuid() uses
  // UuidCreate().
  GUID guid;
  if (CoCreateGuid(&guid) == S_OK) {
    uint8_t* bytes = uuid.bytes();
    bytes[0] = (guid.Data1 >> 24) & 0xff;
    bytes[1] = (guid.Data1 >> 16) & 0xff;
    bytes[2] = (guid.Data1 >> 8) & 0xff;
    bytes[3] = guid.Data1 & 0xff;
    bytes[4] = (guid.Data2 >> 8) & 0xff;
    bytes[5] = guid.Data2 & 0xff;
    bytes[6] = (guid.Data3 >> 8) & 0xff;
    bytes[7] = guid.Data3 & 0xff;
    std::memcpy(bytes+8, guid.Data4, 8);

#if LAF_BASE_TRACE_UUID
    LPOLESTR wstr;
    if (StringFromCLSID(guid, &wstr) == S_OK) {
      std::string str = base::string_to_lower(base::to_utf8(wstr));
      if (!str.empty() && str.front() == '{') str.erase(0, 1);
      if (!str.empty() && str.back() == '}') str.erase(str.size()-1, 1);
      printf("convert_to    = \"%s\"\n"
             "UuidToStringA = \"%s\"\n",
             base::convert_to<std::string>(uuid).c_str(),
             str.c_str());
      CoTaskMemFree(wstr);
    }
#endif
  }
  return uuid;
}

} // namespace base
