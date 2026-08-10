cmake_minimum_required(VERSION 3.20)
project(veroot)

include(CTest)

add_subdirectory(webplatform)
add_subdirectory(voidbrowser)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

