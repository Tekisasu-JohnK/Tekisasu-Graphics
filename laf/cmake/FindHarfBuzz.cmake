# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindHarfBuzz
# ------------
#
# Find the HarfBuzz text shaping engine includes and library.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` target:
#
# ``HarfBuzz::HarfBuzz``
#   The Harfbuzz ``harfbuzz`` library, if found
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``HARFBUZZ_FOUND``
#   true if the HarfBuzz headers and libraries were found
# ``HARFBUZZ_INCLUDE_DIRS``
#   directories containing the Harfbuzz headers.
# ``HARFBUZZ_LIBRARIES``
#   the library to link against
# ``HARFBUZZ_VERSION_STRING``
#   the version of harfbuzz found

# Created by Ebrahim Byagowi.

set(HARFBUZZ_FIND_ARGS
  HINTS
    ENV HARFBUZZ_DIR
  PATHS
    ENV GTKMM_BASEPATH
    [HKEY_CURRENT_USER\\SOFTWARE\\gtkmm\\2.4;Path]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\gtkmm\\2.4;Path]
)

find_path(
  HARFBUZZ_INCLUDE_DIRS
  hb.h
  ${HARFBUZZ_FIND_ARGS}
  PATH_SUFFIXES
    src
    harfbuzz
)

if(NOT HARFBUZZ_LIBRARY)
  find_library(HARFBUZZ_LIBRARY
    NAMES
      harfbuzz
      libharfbuzz
    ${HARFBUZZ_FIND_ARGS}
    PATH_SUFFIXES
      lib
  )
else()
  # on Windows, ensure paths are in canonical format (forward slahes):
  file(TO_CMAKE_PATH "${HARFBUZZ_LIBRARY}" HARFBUZZ_LIBRARY)
endif()

unset(HARFBUZZ_FIND_ARGS)

# set the user variables
set(HARFBUZZ_LIBRARIES "${HARFBUZZ_LIBRARY}")
