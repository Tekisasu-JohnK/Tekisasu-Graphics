// LAF Base Library
// Copyright (C) 2019-2023  Igara Studio S.A.
// Copyright (C) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_THREAD_H_INCLUDED
#define BASE_THREAD_H_INCLUDED
#pragma once

#include <string>

namespace base {
namespace this_thread {

void yield();

// TODO replace with std::this_thread::sleep_for(std::chrono::seconds(...)) or similar
void sleep_for(double seconds);

// Associates a name/description to the current thread. Useful for
// debugging purposes. E.g. When we receive a crash dump from Sentry
// we can identify a thread by its name.
void set_name(const std::string& name);
std::string get_name();

} // this_thread
} // base

#endif
