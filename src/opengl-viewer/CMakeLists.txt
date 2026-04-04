cmake_minimum_required(VERSION 3.16)

project(OpenGLViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)

# 添加Assimp库
set(assimp_DIR "D:/code/vcpkg/installed/x64-windows/share/assimp")
find_package(assimp REQUIRED)

set(SOURCES
    GLWidget.cpp
    GLRenderer.cpp
    GLModel.cpp
    GLCamera.cpp
)

set(HEADERS
    GLWidget.h
    GLRenderer.h
    GLModel.h
    GLCamera.h
)

add_library(OpenGLViewer
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(OpenGLViewer PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    assimp::assimp
)

# 复制着色器文件到构建目录
set(SHADER_FILES
    shaders/solid.vert
    shaders/solid.frag
    shaders/wireframe.vert
    shaders/wireframe.frag
    shaders/point.vert
    shaders/point.frag
)

foreach(SHADER ${SHADER_FILES})
    configure_file(${SHADER} ${CMAKE_BINARY_DIR}/${SHADER} COPYONLY)
endforeach()

# 安装规则
install(TARGETS OpenGLViewer
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# 安装着色器文件
install(DIRECTORY shaders
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

# 安装头文件
install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/opengl-viewer
)