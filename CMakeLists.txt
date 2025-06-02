cmake_minimum_required(VERSION 3.28)

project(DreamDC LANGUAGES CXX VERSION 0.0.1)

find_package(Vulkan REQUIRED)
find_package(glfw REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(DreamDC src/main.cpp)

target_include_directories(DreamDC PUBLIC
    include
    # "${PROJECT_BINARY_DIR}"
)

add_subdirectory(src)

