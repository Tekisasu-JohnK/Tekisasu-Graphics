# LAF
# Copyright (C) 2019-2022  Igara Studio S.A.
# Copyright (C) 2016-2018  David Capello

cmake_minimum_required(VERSION 3.16)

# We need to add the -MT flag for MSVC because we compile Skia with
# the static version of the libc runtime
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(laf C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Search in the "cmake" directory for additional CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Options
set(LAF_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Root LAF CMakeLists.txt file location")
set(LAF_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root location of LAF binary files")

if(SKIA_DIR)
  set(LAF_DEFAULT_BACKEND "skia")
else()
  set(LAF_DEFAULT_BACKEND "none")
endif()

option(LAF_WITH_EXAMPLES "Enable LAF examples" ON)
option(LAF_WITH_TESTS "Enable LAF tests" ON)
set(LAF_BACKEND ${LAF_DEFAULT_BACKEND} CACHE STRING "Select laf backend")
set_property(CACHE LAF_BACKEND PROPERTY STRINGS "none" "skia")
if(NOT APPLE AND UNIX)
option(LAF_OS_WITH_GTK "Use GTK library for native dialogs" OFF)
endif()

# Testing
if(LAF_WITH_TESTS)
  enable_testing()
  include(LafFindTests)
endif()

# Find libraries
if(LAF_BACKEND STREQUAL "skia")
  include(FindSkia)
else()
  if(NOT FREETYPE_LIBRARIES)
    find_package(Freetype)
  endif()
  if(NOT HARFBUZZ_LIBRARIES)
    find_package(HarfBuzz)
  endif()
endif()

add_subdirectory(third_party)
add_subdirectory(base)
add_subdirectory(gfx)
if(FREETYPE_LIBRARIES AND HARFBUZZ_LIBRARIES)
  add_subdirectory(ft)
endif()
add_subdirectory(os)
if(LAF_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

if(LAF_BACKEND STREQUAL "skia")
  target_compile_definitions(laf-base PUBLIC LAF_SKIA)

  # Flag for harfbuzz
  target_compile_definitions(laf-base INTERFACE -DHAVE_CONFIG_OVERRIDE_H=1)
endif()

# Information

message(STATUS "laf backend: ${LAF_BACKEND}")
message(STATUS "laf zlib: ${ZLIB_LIBRARIES}")
message(STATUS "laf pixman: ${PIXMAN_LIBRARY}")
message(STATUS "laf freetype: ${FREETYPE_LIBRARIES}")
message(STATUS "laf harfbuzz: ${HARFBUZZ_LIBRARIES}")
if(LAF_BACKEND STREQUAL "skia")
  message(STATUS "skia dir: ${SKIA_DIR}")
  message(STATUS "skia library: ${SKIA_LIBRARY}")
  message(STATUS "skia library dir: ${SKIA_LIBRARY_DIR}")
  if(NOT SKIA_DIR OR
     NOT SKIA_LIBRARY OR
     NOT SKIA_LIBRARY_DIR)
    message(FATAL_ERROR "set SKIA_DIR/SKIA_LIBRARY/SKIA_LIBRARY_DIR to compile w/skia backend")
  endif()
endif()
