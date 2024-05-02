project(math_test
        LANGUAGES CXX)

find_package(GTest REQUIRED)

enable_testing()

set(SOURCES
    equation_test.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  PRIVATE GTest::gtest_main base math)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
