add_library(multimedia STATIC
    VideoFrameImageProvider.h
    VideoFrameImageProvider.cpp
    VideoPlayer.h
    VideoPlayer.cpp
    FFmpegDecoder.h
    FFmpegDecoder.cpp
    AudioPlayer.h
    AudioPlayer.cpp
)

# FFmpeg 导入目标由父级 src/CMakeLists.txt 通过 FFMPEGImports.cmake 创建

target_link_libraries(multimedia PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Multimedia
)
# FFmpeg 必须 PUBLIC：MSVC 链接静态库 multimedia 时需从下游解析 FFmpeg 符号
target_link_libraries(multimedia PUBLIC
    FFmpeg::avcodec
    FFmpeg::avformat
    FFmpeg::avutil
    FFmpeg::swscale
    FFmpeg::swresample
)

target_include_directories(multimedia PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
