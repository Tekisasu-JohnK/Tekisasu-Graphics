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
  #include <windows.h>
  #include <process.h>
#else
  #include <pthread.h>     // Use pthread library in Unix-like systems

  #include <unistd.h>
  #include <sys/time.h>
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

} // namespace base
