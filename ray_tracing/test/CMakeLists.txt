cmake_minimum_required(VERSION 3.25)
project(ray_tracing_test
        LANGUAGES CXX)

set(CXX_CLANG_TIDY_VALUE ${CMAKE_CXX_CLANG_TIDY})
unset(CMAKE_CXX_CLANG_TIDY)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5df0241ea4880e5a846775d3efc8b873f7b36c31.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(CMAKE_CXX_CLANG_TIDY ${CXX_CLANG_TIDY_VALUE})

set(SOURCES
    cylinder_z_infinite.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE GTest::gtest_main ray_tracing)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
