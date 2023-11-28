// LAF Base Library
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_MEMORY_H_INCLUDED
#define BASE_MEMORY_H_INCLUDED
#pragma once

#include <cstddef>

#ifdef __cplusplus
  #ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
    static constexpr size_t base_alignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
  #else
    static constexpr size_t base_alignment = 1;
  #endif

  constexpr size_t base_align_size(const size_t n,
                                   const size_t alignment = base_alignment) {
    size_t remaining = (n % alignment);
    size_t aligned_n = n + (remaining ? (alignment - remaining): 0);
    if (aligned_n > alignment)
      return aligned_n;
    else
      return alignment;
  }
#endif

void* base_malloc (std::size_t bytes);
void* base_malloc0(std::size_t bytes);
void* base_realloc(void* mem, std::size_t bytes);
void  base_free   (void* mem);
char* base_strdup (const char* string);

#ifdef __cplusplus
  void* base_aligned_alloc(std::size_t bytes, std::size_t alignment = base_alignment);
#else
  void* base_aligned_alloc(std::size_t bytes, std::size_t alignment);
#endif
void base_aligned_free(void* mem);

#ifdef LAF_MEMLEAK
void base_memleak_init();
void base_memleak_exit();
#endif

#endif
