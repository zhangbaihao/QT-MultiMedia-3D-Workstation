cmake_minimum_required(VERSION 3.16)

# vcpkg：与 cmake/FFMPEGImports.cmake 共用 MY_VCPKG_ROOT
if(NOT DEFINED MY_VCPKG_ROOT OR MY_VCPKG_ROOT STREQUAL "")
    set(MY_VCPKG_ROOT "D:/code/vcpkg" CACHE PATH "vcpkg installation root")
endif()
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${MY_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain")
endif()

# Qt 路径（按本机安装修改）
set(Qt6_DIR "C:/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6")

project(QT-MultiMedia-3D-Workstation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(Qt6 COMPONENTS Core Gui Qml Quick OpenGL Widgets Multimedia Quick3D REQUIRED)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MP")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# 包含目录
include_directories(include)

# 子目录
add_subdirectory(src)
#add_subdirectory(tests)

# 打包功能将在 src/CMakeLists.txt 中配置
