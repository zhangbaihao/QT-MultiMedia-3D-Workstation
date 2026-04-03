add_library(3d STATIC
    Qml3DRenderer.h
    Qml3DRenderer.cpp
    ModelLoader.h
    ModelLoader.cpp
    Camera.h
    Camera.cpp
)

target_link_libraries(3d PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
