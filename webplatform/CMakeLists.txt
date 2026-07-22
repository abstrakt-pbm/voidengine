cmake_minimum_required(VERSION 3.20)

project(voidengine_webplatform LANGUAGES CXX)

set(VEWP_SRC
    frame/frame.cc
    frame/framenavigator.cc
    frame/document.cc
    frame/documentparcer.cc
    frame/resourceloader.cc
    frame/htmltokenizer.cc
    frame/htmltoken.cc
)

add_library(voidengine_webplatform STATIC
    ${VEWP_SRC}
)

add_library(VoidEngine::WebPlatform
    ALIAS voidengine_webplatform
)

target_include_directories(voidengine_webplatform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_features(voidengine_webplatform
    PUBLIC
        cxx_std_20
)
