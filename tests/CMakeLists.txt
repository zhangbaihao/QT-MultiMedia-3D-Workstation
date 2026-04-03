enable_testing()

find_package(Qt6 COMPONENTS Core Test REQUIRED)

add_executable(test_core
    test_core.cpp
)

target_link_libraries(test_core PRIVATE
    Qt6::Core
    Qt6::Test
    core
)

add_test(NAME test_core COMMAND test_core)

add_executable(test_3d
    test_3d.cpp
)

target_link_libraries(test_3d PRIVATE
    Qt6::Core
    Qt6::Test
    3d
)

add_test(NAME test_3d COMMAND test_3d)

add_executable(test_multimedia
    test_multimedia.cpp
)

target_link_libraries(test_multimedia PRIVATE
    Qt6::Core
    Qt6::Test
    multimedia
)

add_test(NAME test_multimedia COMMAND test_multimedia)
