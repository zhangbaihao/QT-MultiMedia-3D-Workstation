add_library(memory STATIC
    MemoryPool.h
    MemoryPool.cpp
    RingBuffer.h
    RingBuffer.cpp
)

target_link_libraries(memory PRIVATE
    Qt6::Core
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
