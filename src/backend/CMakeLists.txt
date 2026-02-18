cmake_minimum_required(VERSION 3.12)

project(CompilerBackend
        VERSION 0.0
        DESCRIPTION "Compiler"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(CANON)
target_sources(
    CANON
PRIVATE
    canon/canon.cpp
)
target_include_directories(
    CANON
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/backend/canon>
    $<INSTALL_INTERFACE:include>
)
add_library(CFG)
target_sources(
    CFG
PRIVATE
    cfg/bg.cpp
    cfg/flowgraph.cpp
    cfg/graph.cpp
    cfg/ig.cpp
    cfg/assem_bg.cpp
)  
target_include_directories(
    CFG
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/backend/cfg>
    $<INSTALL_INTERFACE:include>
)

add_library(SSA)
target_sources(
    SSA
PRIVATE
    ssa/domi_tree.cpp
    ssa/build_ssa.cpp
    optimizer/cp.cpp
    optimizer/cdg.cpp
    optimizer/de.cpp
    optimizer/fi.cpp
    optimizer/gcm.cpp
    optimizer/loop.cpp
    optimizer/rp.cpp
    optimizer/gvc.cpp
    optimizer/mc.cpp
    optimizer/gvn.cpp
    optimizer/am.cpp
    optimizer/pattern.cpp
    optimizer/peephole.cpp


)  
target_include_directories(
    SSA
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/backend/cfg>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/structure/mir>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    SSA
PRIVATE
    MIR
)


add_library(ALLOC)
target_sources(
    ALLOC
PRIVATE
    alloc/regalloc.cpp
    alloc/color.cpp
)
target_include_directories(
    ALLOC
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/backend/alloc>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    ALLOC
PRIVATE
    CFG
    ASSEM
)
