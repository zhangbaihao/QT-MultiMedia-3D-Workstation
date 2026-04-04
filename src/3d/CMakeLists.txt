add_library(3d STATIC
    Qml3DRenderer.h
    Qml3DRenderer.cpp
    ModelLoader.h
    ModelLoader.cpp
    Camera.h
    Camera.cpp
    Custom3DModel.h
    Custom3DModel.cpp
)

target_link_libraries(3d PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Quick3D
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
