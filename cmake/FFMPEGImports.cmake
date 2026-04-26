# FFMPEGImports.cmake
# 绕过 vcpkg 和 Qt 的 FindFFmpeg 模块冲突，直接从 vcpkg 路径创建导入目标
# 调用方不需要再执行 find_package(FFMPEG)

include_guard(GLOBAL)

if(NOT DEFINED MY_VCPKG_ROOT OR MY_VCPKG_ROOT STREQUAL "")
    set(MY_VCPKG_ROOT "D:/code/vcpkg" CACHE PATH "vcpkg installation root (contains installed/x64-windows)")
endif()

set(_vcpkg_installed "${MY_VCPKG_ROOT}/installed/x64-windows")

# 查找头文件目录
find_path(FFMPEG_INCLUDE_DIR
    NAMES libavcodec/avcodec.h
    PATHS "${_vcpkg_installed}/include"
    NO_DEFAULT_PATH
)

if(NOT FFMPEG_INCLUDE_DIR)
    message(FATAL_ERROR "未找到 FFmpeg 头文件: ${_vcpkg_installed}/include/libavcodec/avcodec.h (检查 MY_VCPKG_ROOT=${MY_VCPKG_ROOT})")
endif()

# 辅助函数：为单个 FFmpeg 库创建 UNKNOWN IMPORTED 目标
function(_ffmpeg_import libname)
    find_library(_ff_release
        NAMES ${libname}
        PATHS "${_vcpkg_installed}/lib"
        NO_DEFAULT_PATH
    )
    find_library(_ff_debug
        NAMES ${libname} "${libname}d"
        PATHS "${_vcpkg_installed}/debug/lib"
        NO_DEFAULT_PATH
    )

    if(NOT _ff_release AND NOT _ff_debug)
        message(FATAL_ERROR "未找到 FFmpeg 库: ${libname} (MY_VCPKG_ROOT=${MY_VCPKG_ROOT})")
    endif()

    if(TARGET FFmpeg::${libname})
        return()
    endif()

    add_library(FFmpeg::${libname} UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::${libname} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIR}"
    )

    if(_ff_release)
        set_target_properties(FFmpeg::${libname} PROPERTIES
            IMPORTED_LOCATION_RELEASE        "${_ff_release}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${_ff_release}"
            IMPORTED_LOCATION_MINSIZEREL     "${_ff_release}"
        )
    endif()
    if(_ff_debug)
        set_target_properties(FFmpeg::${libname} PROPERTIES
            IMPORTED_LOCATION_DEBUG "${_ff_debug}"
        )
    endif()

    # 处理依赖（函数剩余参数）
    set(_deps ${ARGN})
    if(_deps)
        target_link_libraries(FFmpeg::${libname} INTERFACE ${_deps})
    endif()
endfunction()

# ---- 创建各库导入目标 ----

_ffmpeg_import(avutil)
_ffmpeg_import(avcodec FFmpeg::avutil)
_ffmpeg_import(avformat FFmpeg::avcodec FFmpeg::avutil)
_ffmpeg_import(swscale  FFmpeg::avutil)
_ffmpeg_import(swresample FFmpeg::avutil)
_ffmpeg_import(avfilter FFmpeg::avutil FFmpeg::avcodec FFmpeg::swscale FFmpeg::swresample)
_ffmpeg_import(avdevice FFmpeg::avformat FFmpeg::avcodec FFmpeg::avutil)

# ---- Windows 系统依赖 ----

if(WIN32)
    target_link_libraries(FFmpeg::avformat INTERFACE secur32 ncrypt crypt32)
    target_link_libraries(FFmpeg::avcodec INTERFACE bcrypt)
endif()

# 设置与 vcpkg FindFFMPEG 兼容的变量（如果其他代码需要）
set(FFMPEG_FOUND TRUE CACHE BOOL "FFmpeg found" FORCE)
set(FFMPEG_INCLUDE_DIRS "${FFMPEG_INCLUDE_DIR}" CACHE STRING "FFmpeg include directories" FORCE)

unset(_vcpkg_installed)
unset(_ff_release)
unset(_ff_debug)
