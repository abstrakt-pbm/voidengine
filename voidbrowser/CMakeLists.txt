cmake_minimum_required(VERSION 3.20)
project(VoidBrowser)

add_executable(voidbrowser main.cpp)
target_link_libraries(voidbrowser
    PRIVATE
        VoidEngine::WebPlatform
)
