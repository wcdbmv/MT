cmake_minimum_required(VERSION 3.25)
project(modeling
        LANGUAGES CXX)

set(HEADERS
    include/modeling/fibonacci_sphere.h
)

set(SOURCES
    src/fibonacci_sphere.cc
)

add_library(${PROJECT_NAME} STATIC
            ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE base math)
