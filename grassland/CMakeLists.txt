cmake_minimum_required(VERSION 3.22)

project(grassland)

include_directories(include)

add_executable(grassland)

target_sources(grassland PRIVATE main.cpp)
target_sources(grassland PRIVATE include/GrasslandDecl.cpp)
target_sources(grassland PRIVATE include/Graphics/Graphics.cpp)
target_sources(grassland PRIVATE include/Graphics/OpenGL/GraphicsOpenGL.cpp)
target_sources(grassland PRIVATE include/Graphics/Util/Color.cpp)
target_sources(grassland PRIVATE include/Graphics/Util/Camera.cpp)
target_sources(grassland PRIVATE include/Graphics/Util/Image.cpp)
target_sources(grassland PRIVATE include/Graphics/Util/Mesh.cpp)
target_sources(grassland PRIVATE include/Math/MathDecl.cpp)
target_sources(grassland PRIVATE include/String/String.cpp)

if(MSVC)
    target_sources(grassland PRIVATE include/Graphics/DirectX/GraphicsDirectX.cpp)
    target_link_libraries(grassland d3d12 d3dcompiler dxgi)
endif()

target_link_libraries(grassland freetype glfw glad)
