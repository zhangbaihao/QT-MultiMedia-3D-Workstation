add_library(core STATIC
    Core.h
    Core.cpp
    Model.h
    Model.cpp
    ViewModel.h
    ViewModel.cpp
)

target_link_libraries(core PRIVATE
    Qt6::Core
    Qt6::Gui
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
