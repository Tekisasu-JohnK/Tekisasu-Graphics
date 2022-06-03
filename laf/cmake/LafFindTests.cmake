# Copyright (C) 2019  Igara Studio S.A.
# Copyright (C) 2016  David Capello
# Find tests and add rules to compile them and run them

function(laf_find_tests dir dependencies)
  file(GLOB tests ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*_tests.cpp)
  list(REMOVE_AT ARGV 0)

  # Add gtest include directory so we can #include <gtest/gtest.h> in tests source code
  include_directories(${LAF_ROOT_DIR}/third_party/googletest/googletest/include)

  foreach(testsourcefile ${tests})
    get_filename_component(testname ${testsourcefile} NAME_WE)

    add_executable(${testname} ${testsourcefile})
    add_test(NAME ${testname} COMMAND ${testname})

    if(MSVC)
      set_target_properties(${testname}
        PROPERTIES LINK_FLAGS -ENTRY:"mainCRTStartup")
    endif()

    target_link_libraries(${testname} gtest ${ARGV} ${LAF_OS_PLATFORM_LIBS})

    if(extra_definitions)
      set_target_properties(${testname}
        PROPERTIES COMPILE_FLAGS ${extra_definitions})
    endif()
  endforeach()
endfunction()
