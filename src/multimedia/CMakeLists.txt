add_library(multimedia STATIC
    VideoPlayer.h
    VideoPlayer.cpp
    AudioPlayer.h
    AudioPlayer.cpp
)

target_link_libraries(multimedia PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Multimedia
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
