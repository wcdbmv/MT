project(physics_test
        LANGUAGES CXX)

find_package(GTest REQUIRED)

enable_testing()

set(HEADERS
    expect_vector_near.h
)

set(SOURCES
    expect_vector_near.cc
    refract.cc
    reflect.cc
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  PRIVATE GTest::gtest_main base math physics)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
