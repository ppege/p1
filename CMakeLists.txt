cmake_minimum_required(VERSION 3.14)
project(p1 C)
set(CMAKE_C_STANDARD 11)

# Include dependencies
# include(cmake/mtest.cmake)

add_subdirectory(src)
add_subdirectory(lib)

include(CTest)
enable_testing()
add_subdirectory(test)
