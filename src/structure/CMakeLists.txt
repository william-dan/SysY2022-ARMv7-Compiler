cmake_minimum_required(VERSION 3.12)

project(CompilerStructure
        VERSION 0.0
        DESCRIPTION "CompilerStructure"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(AST)
target_sources(
    AST
PRIVATE
    ast/ast.cpp
)
target_include_directories(
    AST
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/structure/ast>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    AST
PRIVATE
    Utils
)  

add_library(ASSEM)
target_sources(
    ASSEM
PRIVATE
    assem/assem.cpp
)
target_include_directories(
    ASSEM
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/structure/assem>
    $<INSTALL_INTERFACE:include>
)

add_library(MIR)
target_sources(
    MIR
PRIVATE
    mir/mir.cpp
    mir/mir2assem.cpp
    mir/mirflatten.cpp
    mir/mirprint.cpp
    mir/mirfix.cpp
    mir/mirdefuse.cpp
)
target_include_directories(
    MIR
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/structure/mir>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    MIR
PRIVATE
    Utils
    ASSEM
)

