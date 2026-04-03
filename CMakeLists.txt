cmake_minimum_required(VERSION 3.16)

project(QT-MultiMedia-3D-Workstation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指定vcpkg和Qt路径
set(CMAKE_TOOLCHAIN_FILE "D:/code/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(Qt6_DIR "C:/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6")

find_package(Qt6 COMPONENTS Core Gui Qml Quick OpenGL Widgets REQUIRED)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MP")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# 包含目录
include_directories(include)

# 子目录
add_subdirectory(src)
add_subdirectory(tests)
