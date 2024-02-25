cmake_minimum_required(VERSION 3.25)
project(MT
        HOMEPAGE_URL https://github.com/wcdbmv/MT
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions("$<$<CONFIG:DEBUG>:DEBUG>")
add_compile_options("$<$<CONFIG:RELEASE>:-O3;-ffast-math>")

include(cmake/CompileOptions.cmake)
include(cmake/IPO.LTO.cmake)

set(CMAKE_CXX_CLANG_TIDY clang-tidy -warnings-as-errors=*)

# add_subdirectory(ray_tracing)
add_subdirectory(base)
add_subdirectory(examples)
add_subdirectory(geogebra)
add_subdirectory(math)
add_subdirectory(physics)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME} PRIVATE base geogebra math physics)

enable_ipo_lto_for_release(${PROJECT_NAME})
