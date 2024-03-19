cmake_minimum_required(VERSION 3.25)
project(ray_tracing
        LANGUAGES CXX)

set(HEADERS
    include/ray_tracing/cylinder_z.h
    include/ray_tracing/cylinder_z_infinite.h
    include/ray_tracing/disk.h
    include/ray_tracing/plane.h
    include/ray_tracing/shape.h
)

set(SOURCES
    src/cylinder_z.cc
    src/cylinder_z_infinite.cc
    src/disk.cc
    src/plane.cc
    src/shape.cc
)

add_library(${PROJECT_NAME} STATIC
            ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
                      PUBLIC base math)

add_subdirectory(test)
