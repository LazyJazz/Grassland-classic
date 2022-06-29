cmake_minimum_required(VERSION 3.22)

project(grassland)

add_subdirectory(glfw)
add_subdirectory(freetype)
add_subdirectory(glad)

include_directories(glfw/include)
include_directories(freetype/include)
include_directories(glad/include)

add_subdirectory(grassland)
