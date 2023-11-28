// LAF Base Library
// Copyright (C) 2019-2023  Igara Studio S.A.
// Copyright (C) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/thread.h"

#if LAF_WINDOWS
  #include "base/dll.h"
  #include "base/string.h"

  #include <windows.h>
  #include <process.h>
  #include <mutex>
#else
  #include <pthread.h>     // Use pthread library in Unix-like systems

  #include <unistd.h>
  #include <sys/time.h>
  #include <algorithm>
#endif

#if LAF_WINDOWS
namespace {

class KernelBaseApi {
public:
  using SetThreadDescription_Func = HRESULT(WINAPI*)(HANDLE, PCWSTR);
  using GetThreadDescription_Func = HRESULT(WINAPI*)(HANDLE, PWSTR*);

  SetThreadDescription_Func SetThreadDescription = nullptr;
  GetThreadDescription_Func GetThreadDescription = nullptr;

  KernelBaseApi() {
    m_dll = base::load_dll("KernelBase.dll");
    if (m_dll) {
      SetThreadDescription = base::get_dll_proc<SetThreadDescription_Func>(m_dll, "SetThreadDescription");
      GetThreadDescription = base::get_dll_proc<GetThreadDescription_Func>(m_dll, "GetThreadDescription");
    }
  }

private:
  base::dll m_dll;
};

KernelBaseApi kernelBaseApi;

} // anonymous namespace
#endif

namespace base {

void this_thread::yield()
{
#if LAF_WINDOWS

  ::Sleep(0);

#elif defined(HAVE_SCHED_YIELD) && defined(_POSIX_PRIORITY_SCHEDULING)

  sched_yield();

#else

  timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  select(0, nullptr, nullptr, nullptr, &timeout);

#endif
}

void this_thread::sleep_for(double seconds)
{
#if LAF_WINDOWS

  ::Sleep(DWORD(seconds * 1000.0));

#else

  usleep(seconds * 1000000.0);

#endif
}

void this_thread::set_name(const std::string& name)
{
#if LAF_WINDOWS
  if (kernelBaseApi.SetThreadDescription)
    kernelBaseApi.SetThreadDescription(GetCurrentThread(),
                                       base::from_utf8(name).c_str());
#elif LAF_MACOS
  // macOS has a non-standard pthread_setname_np() impl
  int res = pthread_setname_np(name.c_str());
  if (res != 0 && name.size() > 63) {
    pthread_setname_np(name.substr(0, 63).c_str());
  }
#else
  int res = pthread_setname_np(pthread_self(), name.c_str());
  if (res != 0 && name.size() > 15) {
    // Try with a shorter string (no more than 16 chars including the
    // null char, as the spec says).
    pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
  }
#endif
}

std::string this_thread::get_name()
{
#if LAF_WINDOWS
  if (kernelBaseApi.GetThreadDescription) {
    PWSTR desc = nullptr;
    HRESULT hr = kernelBaseApi.GetThreadDescription(GetCurrentThread(), &desc);
    if (SUCCEEDED(hr) && desc) {
      std::string result(base::to_utf8(desc));
      LocalFree(desc);
      return result;
    }
  }
#else
  char name[65];
  int result = pthread_getname_np(pthread_self(), name, sizeof(name)-1);
  if (result == 0) { // Returns 0 if it was successful
    // pthread_getname_np() returns a null terminated name.
    return std::string(name);
  }
#endif
  return std::string();
}

} // namespace base
