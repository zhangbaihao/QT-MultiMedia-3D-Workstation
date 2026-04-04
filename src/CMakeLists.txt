# 主应用程序
add_executable(QT-MultiMedia-3D-Workstation
    main.cpp
    qml.qrc
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
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Quick3D
    qtquick3d-viewer
    multimedia
    imageprocessing
    thread
    memory
)

# 打包功能
if(WIN32)
    # 复制 Qt 依赖库
    set(QT_DLLS
        ${Qt6_DIR}/../../../bin/Qt6Core.dll
        ${Qt6_DIR}/../../../bin/Qt6Gui.dll
        ${Qt6_DIR}/../../../bin/Qt6Qml.dll
        ${Qt6_DIR}/../../../bin/Qt6Quick.dll
        ${Qt6_DIR}/../../../bin/Qt6OpenGL.dll
        ${Qt6_DIR}/../../../bin/Qt6Widgets.dll
        ${Qt6_DIR}/../../../bin/Qt6Multimedia.dll
        ${Qt6_DIR}/../../../bin/Qt6Quick3D.dll
        ${Qt6_DIR}/../../../bin/Qt6Network.dll
        ${Qt6_DIR}/../../../bin/Qt6OpenGLWidgets.dll
        ${Qt6_DIR}/../../../bin/Qt6QmlModels.dll
        ${Qt6_DIR}/../../../bin/Qt6QmlWorkerScript.dll
        ${Qt6_DIR}/../../../bin/Qt6QuickControls2.dll
        ${Qt6_DIR}/../../../bin/Qt6QuickTemplates2.dll
        ${Qt6_DIR}/../../../bin/Qt6QuickWindow.dll
        ${Qt6_DIR}/../../../bin/Qt6ShaderTools.dll
    )
    
    # 复制 vcpkg 依赖库
    set(VCPKG_DLLS
        D:/code/vcpkg/installed/x64-windows/bin/assimp-vc143-mtd.dll
    )
    
    # 复制依赖库到输出目录
    foreach(DLL ${QT_DLLS} ${VCPKG_DLLS})
        if(EXISTS ${DLL})
            add_custom_command(
                TARGET QT-MultiMedia-3D-Workstation POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
            )
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
