project(ray_tracing_test
        LANGUAGES CXX)

find_package(GTest REQUIRED)

enable_testing()

set(SOURCES
    cylinder_z_infinite.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  PRIVATE GTest::gtest_main base math ray_tracing)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
