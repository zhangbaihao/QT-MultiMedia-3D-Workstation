add_library(ui STATIC)

target_link_libraries(ui PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
