include_directories(${PROJECT_SOURCE_DIR}/sources)
include_directories(${PROJECT_SOURCE_DIR}/sac)

link_directories (${PROJECT_BINARY_DIR}/sac)

file(
        GLOB
        source_files
        *.cpp *.h modes/*.cpp modes/*.h states/*.h states/*.cpp api/*.h api/linux/*.h api/linux/*.cpp
)

add_executable(tilematch ${source_files})

target_link_libraries(tilematch sac GLEW glfw png openal sndfile sqlite3  vorbisidec)
