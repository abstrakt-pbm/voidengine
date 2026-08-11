find_package(GTest REQUIRED)

add_executable(voidengine_tests
    conformance/css/box/margin_test.cpp
    conformance/css/box/height_test.cpp
    conformance/css/box/width_test.cpp
    conformance/css/box/overflow_test.cpp
)

target_link_libraries(voidengine_tests
    PRIVATE
        VoidEngine::WebPlatform
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(voidengine_tests)
