# 主应用程序
add_executable(QT-MultiMedia-3D-Workstation
    main.cpp
    qml.qrc
)

# 查找 Qt 模块
find_package(Qt6 COMPONENTS Core Gui Qml Quick OpenGL OpenGLWidgets Widgets Multimedia Quick3D REQUIRED)

# 添加 Assimp 头文件搜索路径
target_include_directories(QT-MultiMedia-3D-Workstation PRIVATE
    D:/code/vcpkg/installed/x64-windows/include
)

# 核心模块
add_subdirectory(multimedia)
add_subdirectory(imageprocessing)
add_subdirectory(thread)
add_subdirectory(memory)
add_subdirectory(qtquick3d-viewer)
add_subdirectory(opengl-viewer)

# 链接库
target_link_libraries(QT-MultiMedia-3D-Workstation PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Quick3D
    qtquick3d-viewer
    OpenGLViewer
    multimedia
    imageprocessing
    thread
    memory
)

# 打包功能
if(WIN32)
    # 复制 vcpkg 依赖库（根据构建类型选择正确的库文件）
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(VCPKG_BIN_DIR "D:/code/vcpkg/installed/x64-windows/debug/bin")
        set(ASSIMP_DLL "${VCPKG_BIN_DIR}/assimp-vc143-mtd.dll")
    else()
        set(VCPKG_BIN_DIR "D:/code/vcpkg/installed/x64-windows/bin")
        set(ASSIMP_DLL "${VCPKG_BIN_DIR}/assimp-vc143-mt.dll")
    endif()
    
    # Assimp 依赖的其他 DLL 文件
    set(ASSIMP_DEPENDENCIES
        "${VCPKG_BIN_DIR}/kubazip.dll"
        "${VCPKG_BIN_DIR}/minizip.dll"
        "${VCPKG_BIN_DIR}/poly2tri.dll"
        "${VCPKG_BIN_DIR}/pugixml.dll"
    )
    
    # 对于 Debug 构建，还需要 zlibd1.dll
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND ASSIMP_DEPENDENCIES "${VCPKG_BIN_DIR}/zlibd1.dll")
    endif()
    
    # 复制 Assimp 主 DLL
    if(EXISTS ${ASSIMP_DLL})
        add_custom_command(
            TARGET QT-MultiMedia-3D-Workstation POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_DLL}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    else()
        message(WARNING "Assimp DLL not found at: ${ASSIMP_DLL}")
    endif()
    
    # 复制 Assimp 依赖的其他 DLL
    foreach(DLL ${ASSIMP_DEPENDENCIES})
        if(EXISTS ${DLL})
            add_custom_command(
                TARGET QT-MultiMedia-3D-Workstation POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
            )
        else()
            message(WARNING "Assimp dependency DLL not found at: ${DLL}")
        endif()
    endforeach()
    
    # 复制 QML 文件到输出目录
    add_custom_command(
        TARGET QT-MultiMedia-3D-Workstation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/qml" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml"
    )
    
    # 复制测试模型到输出目录
    add_custom_command(
        TARGET QT-MultiMedia-3D-Workstation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test_models" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_models"
    )
endif()

# 资源文件
set(QML_FILES
    qml/main.qml
    qml/MainWindow.qml
    qml/HomePage.qml
    qml/ModelViewerPage.qml
    qml/VideoPlayerPage.qml
    qml/ControlsPage.qml
)

qt_add_qml_module(QT-MultiMedia-3D-Workstation
    URI QtMultiMedia3D
    VERSION 1.0
    QML_FILES ${QML_FILES}
    RESOURCES qml.qrc
)
