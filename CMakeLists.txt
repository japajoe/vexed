cmake_minimum_required(VERSION 3.12)

project(vexed)

add_subdirectory(external/glfw)

file(GLOB_RECURSE SOURCES 
    src/*.cpp 
    src/*c
)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
    "${PROJECT_SOURCE_DIR}/include/glad"
    "${PROJECT_SOURCE_DIR}/include/glfw"
    "${PROJECT_SOURCE_DIR}/include/stb"
    "${PROJECT_SOURCE_DIR}/include/vexed"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

#On windows you only need to link glfw
target_link_libraries(${PROJECT_NAME} glfw -pthread -ldl -lm)