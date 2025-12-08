cmake_minimum_required(VERSION 3.14)
project(p1 C)
set(CMAKE_C_STANDARD 11)

# Option to enable AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
  message(STATUS "AddressSanitizer enabled")
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=address)
endif()

# Include dependencies
# include(cmake/mtest.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(lib)

include(CTest)
enable_testing()
add_subdirectory(test)
