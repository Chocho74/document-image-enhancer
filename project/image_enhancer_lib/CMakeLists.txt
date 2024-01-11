# Not required, but nice to have for completions sake
cmake_minimum_required(VERSION 3.9)
project(image_enhancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# System-agnostic search for OpenMP
find_package(OpenMP REQUIRED)

add_library(image_enhancer STATIC include/image_enhancer.h src/image_enhancer.cpp)

target_include_directories(image_enhancer PUBLIC include PRIVATE src)

target_link_libraries(image_enhancer PUBLIC OpenMP::OpenMP_CXX)
