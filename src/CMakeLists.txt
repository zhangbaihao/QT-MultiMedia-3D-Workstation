# 主应用程序
add_executable(QT-MultiMedia-3D-Workstation
    main.cpp
    qml.qrc
)

# 核心模块
add_subdirectory(core)
add_subdirectory(3d)
add_subdirectory(multimedia)
add_subdirectory(imageprocessing)
add_subdirectory(thread)
add_subdirectory(memory)
add_subdirectory(ui)

# 链接库
target_link_libraries(QT-MultiMedia-3D-Workstation PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::OpenGL
    Qt6::Widgets
    core
    3d
    multimedia
    imageprocessing
    thread
    memory
    ui
)

# 资源文件
set(QML_FILES
    qml/main.qml
)

qt_add_qml_module(QT-MultiMedia-3D-Workstation
    URI QtMultiMedia3D
    VERSION 1.0
    QML_FILES ${QML_FILES}
)
