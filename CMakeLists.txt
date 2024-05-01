cmake_minimum_required(VERSION 3.29.2)
project(MT
        VERSION 0.1
        DESCRIPTION "Master thesis"
        HOMEPAGE_URL https://github.com/wcdbmv/MT
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(MT_ENABLE_CLANG_TIDY "" ON)
option(MT_ENABLE_COMPILER_WARNINGS "" ON)
option(MT_ENABLE_IPO_LTO "" ON)
option(MT_ENABLE_IWYU "" ON)
option(MT_ENABLE_UNSAFE_MATH_OPTIMIZATIONS "" ON)

if(MT_ENABLE_CLANG_TIDY)
  include(cmake/ClangTidy.cmake)
endif()

if(MT_ENABLE_COMPILER_WARNINGS)
  include(cmake/CompileOptions.cmake)
endif()

if(MT_ENABLE_IWYU)
  # TODO(a.kerimov): Update imp database.
  include(cmake/IWYU.cmake)
endif()

if(MT_ENABLE_UNSAFE_MATH_OPTIMIZATIONS)
  add_compile_options("$<$<CONFIG:Release,RelWithDebInfo>:-Ofast>")
endif()

add_executable(${PROJECT_NAME} main.cc)

if(MT_ENABLE_IPO_LTO)
  include(cmake/IPO.LTO.cmake)
  enable_ipo_lto_for_release(${PROJECT_NAME})
endif()
