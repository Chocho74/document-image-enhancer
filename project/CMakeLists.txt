cmake_minimum_required(VERSION 3.9)
project(img_enhancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# System-agnostic search for OpenMP
find_package(OpenMP REQUIRED)

add_subdirectory(image_enhancer_lib)

add_executable(enhance main.cpp)

target_link_libraries(enhance PUBLIC OpenMP::OpenMP_CXX image_enhancer)
