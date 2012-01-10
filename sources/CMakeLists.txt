include_directories(${PROJECT_SOURCE_DIR}/sources)
include_directories(${PROJECT_SOURCE_DIR}/sac)

link_directories (${PROJECT_BINARY_DIR}/sac) 

file(
        GLOB_RECURSE
        source_files
        *
)

add_executable(tilematch ${source_files})

target_link_libraries(tilematch sac glfw png)
