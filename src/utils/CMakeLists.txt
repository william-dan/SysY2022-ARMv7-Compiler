cmake_minimum_required(VERSION 3.12)

project(CompilerUtils
        VERSION 0.0
        DESCRIPTION "CompilerUtils"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Utils)
target_sources(
    Utils
PRIVATE
    entry.hpp
    table.cpp table.hpp
    temp.cpp temp.hpp
    utils.hpp
)
target_include_directories(
    Utils
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/utils>
    $<INSTALL_INTERFACE:include>
)
# add_library(Utils::Utils ALIAS Utils)
