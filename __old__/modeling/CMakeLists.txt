cmake_minimum_required(VERSION 3.25)
project(modeling
        LANGUAGES CXX)

set(HEADERS
    include/modeling/fibonacci_sphere.h
    include/modeling/cylinder_common.h
    include/modeling/cylinder_plasma_quartz.h
    include/modeling/hollow_cylinder.h
    include/modeling/solid_cylinder.h
    include/modeling/worker.h
)

set(SOURCES
    src/fibonacci_sphere.cc
    src/cylinder_plasma_quartz.cc
    src/hollow_cylinder.cc
    src/solid_cylinder.cc
)

add_library(${PROJECT_NAME} STATIC
            ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE base math physics ray_tracing)
