cmake_minimum_required(VERSION 3.20)
project(VoidBrowser)

find_package(SDL3 CONFIG REQUIRED)

add_executable(voidbrowser main.cpp )

target_link_libraries(voidbrowser
    PRIVATE
        VoidEngine::WebPlatform
	SDL3::SDL3
)
