add_library(imageprocessing STATIC
    ImageProcessor.h
    ImageProcessor.cpp
)

target_link_libraries(imageprocessing PRIVATE
    Qt6::Core
    Qt6::Gui
)

# 集成OpenCV（暂时注释，后续可根据实际情况配置）
# find_package(OpenCV REQUIRED)
# target_include_directories(imageprocessing PRIVATE ${OpenCV_INCLUDE_DIRS})
# target_link_libraries(imageprocessing PRIVATE ${OpenCV_LIBS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
