include_directories(${PROJECT_SOURCE_DIR}/sources)
include_directories(${PROJECT_SOURCE_DIR}/sac)

link_directories (${PROJECT_BINARY_DIR}/sac) 

add_executable(tilematch tilematch-linux.cpp)

target_link_libraries(tilematch sac)
