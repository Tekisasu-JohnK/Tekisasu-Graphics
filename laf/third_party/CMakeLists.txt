# LAF
# Copyright (C) 2016-2018  David Capello

# ----------------------------------------------------------------------
# stringencoders library

include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)
if(WORDS_BIGENDIAN)
  add_definitions(-DWORDS_BIGENDIAN)
endif()

configure_file(
  stringencoders-config.h.cmakein
  ${CMAKE_CURRENT_BINARY_DIR}/stringencoders/config.h @ONLY)

include_directories(BEFORE stringencoders/src
  ${CMAKE_CURRENT_BINARY_DIR}/stringencoders)

if(LAF_MODP_B64_GEN)
  add_executable(modp_b64_gen IMPORTED)
  set_target_properties(modp_b64_gen PROPERTIES
    IMPORTED_LOCATION ${LAF_MODP_B64_GEN})
  set(LAF_MODP_B64_GEN_DEP)
else()
  add_executable(modp_b64_gen
    stringencoders/src/arraytoc.c
    stringencoders/src/modp_b64_gen.c)
  set_target_properties(modp_b64_gen
    PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  set(LAF_MODP_B64_GEN modp_b64_gen)
  set(LAF_MODP_B64_GEN_DEP modp_b64_gen)
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/stringencoders/modp_b64_data.h
  DEPENDS ${LAF_MODP_B64_GEN_DEP}
  COMMAND ${LAF_MODP_B64_GEN} > ${CMAKE_CURRENT_BINARY_DIR}/stringencoders/modp_b64_data.h)

add_library(modpbase64
  stringencoders/src/modp_b64.c
  ${CMAKE_CURRENT_BINARY_DIR}/stringencoders/modp_b64_data.h)

target_include_directories(modpbase64 PUBLIC
  stringencoders/src)

# ----------------------------------------------------------------------
# googletest library
if(LAF_WITH_TESTS)
  set(gtest_force_shared_crt ON CACHE BOOL "")
  add_subdirectory(googletest)
endif()
