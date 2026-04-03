add_library(multimedia STATIC
    VideoPlayer.h
    VideoPlayer.cpp
    AudioPlayer.h
    AudioPlayer.cpp
)

target_link_libraries(multimedia PRIVATE
    Qt6::Core
    Qt6::Gui
)

# 集成FFmpeg（暂时注释，后续可根据实际情况配置）
# find_package(FFmpeg REQUIRED)
# target_include_directories(multimedia PRIVATE ${FFMPEG_INCLUDE_DIRS})
# target_link_libraries(multimedia PRIVATE ${FFMPEG_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
