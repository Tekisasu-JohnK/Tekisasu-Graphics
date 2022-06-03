# LAF Gfx Library
# Copyright (c) 2018-2019  Igara Studio S.A.
# Copyright (C) 2001-2017  David Capello

set(LAF_GFX_EXTRA_SOURCES)
if(LAF_BACKEND STREQUAL "skia")
  set(LAF_GFX_EXTRA_SOURCES
    packing_rects.cpp
    region_skia.cpp)
else()
  if(NOT PIXMAN_LIBRARY)
    find_package(Pixman)
  endif()
  if(PIXMAN_LIBRARY)
    set(LAF_GFX_EXTRA_SOURCES
      packing_rects.cpp
      region_pixman.cpp)
  endif()
endif()

add_library(laf-gfx
  color_space.cpp
  hsl.cpp
  hsv.cpp
  rgb.cpp
  ${LAF_GFX_EXTRA_SOURCES})

target_link_libraries(laf-gfx laf-base)
if(LAF_BACKEND STREQUAL "skia")
  # We need Skia for SkRegion
  target_link_libraries(laf-gfx skia)
  target_compile_definitions(laf-gfx PUBLIC LAF_WITH_REGION)
elseif(PIXMAN_LIBRARY)
  target_link_libraries(laf-gfx ${PIXMAN_LIBRARY})
  target_include_directories(laf-gfx PRIVATE ${PIXMAN_INCLUDE_DIR})
  target_compile_definitions(laf-gfx PUBLIC LAF_WITH_REGION LAF_PIXMAN)
endif()

if(LAF_WITH_TESTS)
  laf_find_tests(. laf-gfx)
endif()
