project(base
        LANGUAGES CXX)

set(HEADERS
    include/base/config/build_type.h
    include/base/config/noexcept_release.h
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)
