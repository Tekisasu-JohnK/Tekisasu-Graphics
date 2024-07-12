# cityhash

project(cityhash)

check_cxx_source_compiles("
  int main() { return __builtin_expect(0, 0); }
  " HAVE_BUILTIN_EXPECT)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h" )

add_library(cityhash src/city.cc)
target_include_directories(cityhash PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(cityhash PUBLIC src)
