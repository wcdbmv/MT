cmake_minimum_required(VERSION 3.25)
project(math
        LANGUAGES CXX)

set(HEADERS
    include/math/consts/golden_ratio.h
    include/math/consts/pi.h
    include/math/equation.h
    include/math/fast_pow.h
    include/math/linalg/fibonacci_sphere.h
    include/math/linalg/ray.h
    include/math/linalg/vector.h
    include/math/linalg/vector3f.h
    include/math/linalg/vector_io.h
    include/math/random.h
    include/math/sqrt.h
    include/math/utils.h
)

set(SOURCES
    src/equation.cc
    src/linalg/fibonacci_sphere.cc
    src/random.cc
    src/utils.cc
)

add_library(${PROJECT_NAME} STATIC
            ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
                      PUBLIC base)
