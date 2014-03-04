cmake_minimum_required(VERSION 2.6)

#set the name of the project...
project(Heriswap)

add_definitions(-DSAC_RESTRICTIVE_PLUGINS=1)

#and let the magic begin :-)
include(sac/build/cmake/CMakeLists.txt)


