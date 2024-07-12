// LAF Base Library
// Copyright (c) 2020  Igara Studio S.A.
// Copyright (c) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_BASE_H_INCLUDED
#define BASE_BASE_H_INCLUDED
#pragma once

#include "base/config.h"

#include <math.h>

#ifdef _MSC_VER
  #define LAF_NORETURN(ret, name, args) __declspec(noreturn) ret name args
#else
  #define LAF_NORETURN(ret, name, args) ret name args __attribute__((noreturn))
#endif

#define LAF_ANALYZER_NORETURN
#if defined(__clang__)
  #if __has_feature(attribute_analyzer_noreturn)
    #undef LAF_ANALYZER_NORETURN
    #define LAF_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
  #endif
#endif

#undef NULL
#ifdef __cplusplus
  #define NULL nullptr
#else
  #define NULL ((void*)0)
#endif

#undef ABS
#undef SGN
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#define SGN(x)       (((x) >= 0) ? 1 : -1)



//////////////////////////////////////////////////////////////////////
// Overloaded new/delete operators to detect memory-leaks

#if defined __cplusplus && defined LAF_MEMLEAK

#include <new>

#ifdef _NOEXCEPT
  #define LAF_NOEXCEPT _NOEXCEPT
#else
  #define LAF_NOEXCEPT
#endif

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr) LAF_NOEXCEPT;
void operator delete[](void* ptr) LAF_NOEXCEPT;

#endif

#endif
