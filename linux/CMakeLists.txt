include_directories(${PROJECT_SOURCE_DIR}/sources)
include_directories(${PROJECT_SOURCE_DIR}/sac)
include_directories(${PROJECT_SOURCE_DIR}/sac/libs/squirrel/include)
include_directories(${PROJECT_SOURCE_DIR}/sac/libs/sqrat/include)
link_directories (${PROJECT_BINARY_DIR}/sac)
link_directories (${PROJECT_BINARY_DIR}/sac/libs/squirrel/lib)

file(
        GLOB
        source_files
        *.cpp *.h
)

if (EMSCRIPTEN_BUILD)
	add_executable(heriswap.bc ${source_files})
	target_link_libraries(heriswap.bc heri sac GLEW glfw openal sqlite3  vorbisfile vorbis z)
	add_custom_command(TARGET heriswap.bc
		POST_BUILD
		COMMAND emcc -O1 heriswap.bc -o /tmp/heriswap.html --preload-file ${CMAKE_SOURCE_DIR}/assets
		COMMENT "Building emscripten HTML")	
else ()
	add_executable(heriswap ${source_files})
	target_link_libraries(heriswap heri sac GLEW glfw openal sqlite3  vorbisfile vorbis z)
endif ()
