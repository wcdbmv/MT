cmake_minimum_required(VERSION 3.25)
project(test
        LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(SOURCES
    cylinder.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE GTest::gtest_main convex_hull)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
