cmake_minimum_required(VERSION 3.12)

project(compiler
        VERSION 0.0
        DESCRIPTION "Compiler developed by XFD"
        LANGUAGES C CXX
)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

enable_testing()

# enable c++17 globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE "Debug")

# find and configure required package
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(structure)
add_subdirectory(utils)
# add_subdirectory(tests)

# include_directories(
#   ${CMAKE_SOURCE_DIR}
#   ${CMAKE_SOURCE_DIR}/backend
#   ${CMAKE_SOURCE_DIR}/structure
#   ${CMAKE_SOURCE_DIR}/utils
# )

add_executable(
   ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
  Utils
  ASSEM
  MIR
  CANON
  CFG
  SSA
  ALLOC
  AST
  FRONTEND
)
