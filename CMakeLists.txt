cmake_minimum_required(VERSION 2.6)

#set the name of the project...
project(Heriswap)

add_definitions(
    -DDISABLE_AUTONOMOUS_SYSTEM=1
    -DDISABLE_COLLISION_SYSTEM=1
    -DDISABLE_GRAPH_SYSTEM=1
    -DDISABLE_GRID_SYSTEM=1
    -DDISABLE_DEBUGGING_SYSTEM=1
    -DDISABLE_ZSQD_SYSTEM=1
    -DDISABLE_SPOT_SYSTEM=1
    -DDISABLE_BLINK_SYSTEM=1
    -DDISABLE_DEBUGGING_SYSTEM=1
    -DDISABLE_NETWORK_SYSTEM=1
)

#and let the magic begin :-)
include(sac/CMakeLists.txt)


