project(math
        LANGUAGES CXX)

set(HEADERS
    include/math/consts/golden_ratio.h
    include/math/consts/pi.h
    include/math/equation.h
    include/math/float/compare.h
    include/math/float/eps.h
    include/math/float/exp.h
    include/math/float/log.h
    include/math/float/pow.h
    include/math/float/sqrt.h
    include/math/linalg/vector.h
    include/math/linalg/vector_io.h
)

set(SOURCES
    src/equation.cc
)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
  PRIVATE base)
