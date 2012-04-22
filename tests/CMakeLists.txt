# build test app
file(
	GLOB_RECURSE
	tilematch_tests_files
	*.cpp
)
file(
        GLOB_RECURSE
        tilematch_files
        ../sources/*.cpp
)
message(STATUS "fcu " ${tilematch_tests_files})
message(STATUS "fcu " ${tilematch_files}) 
list(REMOVE_ITEM tilematch_files ${PROJECT_SOURCE_DIR}/tests/../sources/tilematch-linux.cpp) 
message(STATUS "fcu " ${tilematch_files}) 
include_directories(${PROJECT_SOURCE_DIR}/sources)
include_directories(${PROJECT_SOURCE_DIR}/sac)
include_directories("/usr/include/unittest++/")
add_executable(tilematch_tests ${tilematch_tests_files} ${tilematch_files})
target_link_libraries(tilematch_tests sac UnitTest++ GLEW glfw openal sndfile)
add_custom_command(
	TARGET tilematch_tests
	POST_BUILD
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tilematch_tests)
