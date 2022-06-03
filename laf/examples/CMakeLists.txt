# LAF OS
# Copyright (C) 2019-2020  Igara Studio S.A.

if(LAF_BACKEND STREQUAL "skia")
  function(laf_add_example name console="")
    if("${console}" STREQUAL "CONSOLE")
      add_executable(${name} ${name}.cpp)
    else()
      add_executable(${name} WIN32 ${name}.cpp)
    endif()
    target_link_libraries(${name} laf-os)
    set_target_properties(${name} PROPERTIES LINK_FLAGS "${LAF_BACKEND_LINK_FLAGS}")
  endfunction()

  laf_add_example(allevents GUI)
  laf_add_example(complextextlayout GUI)
  laf_add_example(helloworld GUI)
  laf_add_example(listfonts CONSOLE)
  laf_add_example(panviewport GUI)
endif()
