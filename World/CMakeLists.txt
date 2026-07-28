cmake_minimum_required(VERSION 3.10)
project(3DObjectMover LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)


add_executable(${PROJECT_NAME}
    Src/main.cpp
    Src/ModelLoader/3DModelLoader.cpp
    Includes/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE Includes)
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders)