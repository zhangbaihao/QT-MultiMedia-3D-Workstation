enable_testing()

find_package(Qt6 COMPONENTS Core Test Gui Qml Quick Multimedia REQUIRED)

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

# 自动化3D测试
add_executable(auto_test_3d
    auto_test_3d.cpp
)

target_link_libraries(auto_test_3d PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    3d
)

add_test(NAME auto_test_3d COMMAND auto_test_3d)

# 集成测试
add_executable(integration_test_3d
    integration_test_3d.cpp
)

target_link_libraries(integration_test_3d PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    3d
)

add_test(NAME integration_test_3d COMMAND integration_test_3d)

# 视频播放测试
add_executable(test_video
    test_video.cpp
)

target_link_libraries(test_video PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Multimedia
)

add_test(NAME test_video COMMAND test_video)

# 3D功能测试
add_executable(test_3d_functionality
    test_3d_functionality.cpp
)

target_link_libraries(test_3d_functionality PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    3d
)

add_test(NAME test_3d_functionality COMMAND test_3d_functionality)
