cmake_minimum_required(VERSION 3.20)

project(voidengine_webplatform LANGUAGES CXX)

set(VEWP_SRC
    document/div.cc
    document/domnode.cc
    document/containernode.cc
    document/elementnode.cc
    document/textelement.cc
    document/imageelement.cc
    document/htmlelementnode.cc
    document/physicalfragment.cc
    document/style.cc

    html/htmlparser.cc
    html/htmltoken.cc
    html/htmltokenizer.cc
    html/htmltreebuilder.cc

    rendering/geometryengine.cc
    rendering/painterengine.cc
    rendering/layoutcontext.cc
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
