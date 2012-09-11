# build test app
file(
	GLOB_RECURSE
	tilematch_tests_files
	*.cpp
)
include_directories(${PROJECT_SOURCE_DIR}/sources)
include_directories(${PROJECT_SOURCE_DIR}/sac)
include_directories("/usr/include/unittest++/")
add_executable(heriswap_tests ${tilematch_tests_files})
target_link_libraries(heriswap_tests heri sac UnitTest++ GLEW glfw openal z vorbis vorbisfile sqlite3 jsoncpp)
add_custom_command(
	TARGET heriswap_tests
	POST_BUILD
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/heriswap_tests)
