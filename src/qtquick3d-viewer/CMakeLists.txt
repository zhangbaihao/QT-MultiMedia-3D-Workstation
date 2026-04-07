cmake_minimum_required(VERSION 3.16)
# 添加Assimp库
set(assimp_DIR "D:/code/vcpkg/installed/x64-windows/share/assimp")
project(qtquick3d-viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Quick Quick3D REQUIRED)


find_package(assimp REQUIRED)

set(SOURCES
    quickModelLoader.cpp
)

set(HEADERS
    quickModelLoader.h
)

add_library(qtquick3d-viewer STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(qtquick3d-viewer PRIVATE
    Qt6::Quick
    Qt6::Quick3D
    assimp::assimp
)