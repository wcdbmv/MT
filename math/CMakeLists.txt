project(math
        LANGUAGES CXX)

set(HEADERS
    include/math/consts/golden_ratio.h
    include/math/consts/pi.h
    include/math/equation.h
    include/math/fast_pow.h
    include/math/float/compare.h
    include/math/float/eps.h
    include/math/linalg/vector.h
    include/math/linalg/vector_io.h
    include/math/random.h
)

set(SOURCES
    src/equation.cc
    src/random.cc
)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
  PRIVATE base)
