project(math
        LANGUAGES CXX)

set(HEADERS
    include/math/types/float.h
    include/math/types/float_cmp.h
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)
