# LAF FreeType Wrapper
# Copyright (C) 2019-2022  Igara Studio S.A.
# Copyright (C) 2017  David Capello

add_library(laf-ft
  lib.cpp
  stream.cpp)

target_link_libraries(laf-ft
  laf-base
  ${FREETYPE_LIBRARIES}
  ${HARFBUZZ_LIBRARIES})

target_include_directories(laf-ft
  PUBLIC
  ${FREETYPE_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS})
