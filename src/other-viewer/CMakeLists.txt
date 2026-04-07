cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Qml Quick OpenGL Concurrent REQUIRED)

add_library(other-viewer STATIC
    OpenGLRenderer.cpp
    OpenGLRenderer.h
    FramebufferObject.cpp
    FramebufferObject.h
    ModelLoader.cpp
    ModelLoader.h
)

target_include_directories(other-viewer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(other-viewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::OpenGL
    Qt6::Concurrent
    opengl32
)

# 复制着色器文件到输出目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/)
