cmake_minimum_required(VERSION 3.28)

project(DreamDC LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(DreamDC src/main.cpp)

target_include_directories(DreamDC PUBLIC
    include
    # "${PROJECT_BINARY_DIR}"
)

add_subdirectory(src)

### SDL3
if(BUILD_SDL)
	add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
else()
	find_package(SDL3 REQUIRED) # I love nix!!!!
endif()

target_link_libraries(DreamDC PRIVATE SDL3::SDL3)
