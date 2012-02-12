cmake_minimum_required(VERSION 2.8)

project(TILEMATCH)

SET(CMAKE_C_FLAGS "-O0 -g")

add_subdirectory(sources)
add_subdirectory(tests)
add_subdirectory(sac/build/cmake)
