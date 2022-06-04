// LAF Base Library
// Copyright (c) 2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <windows.h>

typedef BOOL (WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
typedef const char* (CDECL* LPFN_WINE_GET_VERSION)(void);

static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
static LPFN_WINE_GET_VERSION fnWineGetVersion = NULL;

namespace base {

bool is_wow64()
{
  if (!fnIsWow64Process) {
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)
      GetProcAddress(GetModuleHandle(L"kernel32"),
                     "IsWow64Process");
  }

  BOOL isWow64 = FALSE;
  if (fnIsWow64Process)
    fnIsWow64Process(GetCurrentProcess(), &isWow64);
  return isWow64 ? true: false;
}

const char* get_wine_version()
{
  if (!fnWineGetVersion) {
    fnWineGetVersion = (LPFN_WINE_GET_VERSION)
      GetProcAddress(GetModuleHandle(L"ntdll.dll"),
                     "wine_get_version");
  }
  if (fnWineGetVersion)
    return fnWineGetVersion();
  else
    return nullptr;
}

} // namespace base
