cmake_minimum_required(VERSION 3.25)
project(math
        LANGUAGES CXX)

set(HEADERS
    include/math/consts/golden_ratio.h
    include/math/consts/pi.h
    include/math/equation.h
    include/math/fast_pow.h
    include/math/geometry/cylinder_z.h
    include/math/geometry/cylinder_z_infinite.h
    include/math/geometry/disk.h
    include/math/geometry/fibonacci_sphere.h
    include/math/geometry/plane.h
    include/math/geometry/ray.h
    include/math/geometry/shape.h
    include/math/geometry/vector.h
    include/math/geometry/vector3f.h
    include/math/geometry/vector_io.h
    include/math/utils.h
)

set(SOURCES
    src/equation.cc
    src/geometry/cylinder_z.cc
    src/geometry/cylinder_z_infinite.cc
    src/geometry/disk.cc
    src/geometry/fibonacci_sphere.cc
    src/geometry/plane.cc
    src/geometry/shape.cc
    src/utils.cc
)

add_library(${PROJECT_NAME} STATIC
            ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
                      PUBLIC base)

add_subdirectory(test)
