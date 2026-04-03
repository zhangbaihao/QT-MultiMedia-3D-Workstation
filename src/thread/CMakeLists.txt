add_library(thread STATIC
    ThreadPool.h
    ThreadPool.cpp
    Task.h
    Task.cpp
)

target_link_libraries(thread PRIVATE
    Qt6::Core
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
