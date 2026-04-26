# vcpkg 工具链由根 CMakeLists.txt 设置；MY_VCPKG_ROOT 与 FFMPEGImports.cmake 一致

# 查找 Qt 模块
find_package(Qt6 COMPONENTS Core Gui Qml Quick Widgets Multimedia Quick3D REQUIRED)

# 主应用程序
add_executable(QT-MultiMedia-3D-Workstation
    main.cpp
    qml.qrc
)



# FFmpeg 库（用于视频播放）
# 使用自定义模块直接从 vcpkg 创建导入目标，绕过 Qt/vcpkg FindFFmpeg 模块冲突
include(${CMAKE_SOURCE_DIR}/cmake/FFMPEGImports.cmake)

# 添加 vcpkg 头文件搜索路径（Assimp 等）
target_include_directories(QT-MultiMedia-3D-Workstation PRIVATE
    "${MY_VCPKG_ROOT}/installed/x64-windows/include"
)

# 核心模块
add_subdirectory(multimedia)
add_subdirectory(imageprocessing)
add_subdirectory(thread)
add_subdirectory(memory)
add_subdirectory(qtquick3d-viewer)
add_subdirectory(other-viewer)

# 链接库
target_link_libraries(QT-MultiMedia-3D-Workstation PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Quick3D
    qtquick3d-viewer
    multimedia
    imageprocessing
    thread
    memory
    other-viewer
    opengl32
)

# 打包功能
if(WIN32)
    # Assimp / FFmpeg 等 DLL 由 vcpkg 工具链的 applocal.ps1 在链接后自动拷贝到可执行文件目录
    set(_TDIR "$<TARGET_FILE_DIR:QT-MultiMedia-3D-Workstation>")

    # 复制 QML 文件到输出目录
    add_custom_command(
        TARGET QT-MultiMedia-3D-Workstation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/qml" "${_TDIR}/qml"
    )
    
    # 复制测试模型到输出目录
    add_custom_command(
        TARGET QT-MultiMedia-3D-Workstation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test_models" "${_TDIR}/test_models"
    )
endif()

# 资源文件
set(QML_FILES
    qml/main.qml
    qml/MainWindow.qml
    qml/HomePage.qml
    qml/ModelViewerPage.qml
    qml/ModelViewerOther.qml
    qml/VideoPlayerPage.qml
    qml/ControlsPage.qml
)

qt_add_qml_module(QT-MultiMedia-3D-Workstation
    URI QtMultiMedia3D
    VERSION 1.0
    QML_FILES ${QML_FILES}
    RESOURCES qml.qrc
)

# FFmpeg：显式按配置链接 .lib（IMPORTED 目标在 VS 多配置 + Qt QML 目标下偶发未进最终链接命令）
set(_FFDBG "${MY_VCPKG_ROOT}/installed/x64-windows/debug/lib")
set(_FFREL "${MY_VCPKG_ROOT}/installed/x64-windows/lib")
target_link_libraries(QT-MultiMedia-3D-Workstation PRIVATE
    "$<IF:$<CONFIG:Debug>,${_FFDBG}/avutil.lib,${_FFREL}/avutil.lib>"
    "$<IF:$<CONFIG:Debug>,${_FFDBG}/avcodec.lib,${_FFREL}/avcodec.lib>"
    "$<IF:$<CONFIG:Debug>,${_FFDBG}/avformat.lib,${_FFREL}/avformat.lib>"
    "$<IF:$<CONFIG:Debug>,${_FFDBG}/swscale.lib,${_FFREL}/swscale.lib>"
    "$<IF:$<CONFIG:Debug>,${_FFDBG}/swresample.lib,${_FFREL}/swresample.lib>"
)
