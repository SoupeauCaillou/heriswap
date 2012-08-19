cmake_minimum_required(VERSION 2.8)

project(TILEMATCH)

SET(CMAKE_C_FLAGS "-O0 -g -W -Wall")
SET(CMAKE_CXX_FLAGS "-O0 -g -W -Wall -DDEBUG -DDATADIR=\\\"${CMAKE_SOURCE_DIR}/assets/\\\"")

add_subdirectory(sources)

add_subdirectory(linux)
if (EMSCRIPTEN_BUILD)
else ()
add_subdirectory(tests)
endif()
add_subdirectory(sac/build/cmake)
