project(math
        LANGUAGES CXX)

set(HEADERS
    include/math/consts/golden_ratio.h
    include/math/consts/pi.h
    include/math/float/compare.h
    include/math/float/eps.h
    include/math/float/exp.h
    include/math/float/log.h
    include/math/float/pow.h
    include/math/float/sqrt.h
    include/math/linalg/vector.h
    include/math/linalg/vector_io.h
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)

target_link_libraries(${PROJECT_NAME}
  INTERFACE base)
