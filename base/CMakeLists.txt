project(base
        LANGUAGES CXX)

set(HEADERS
    include/base/config/build_type.h
    include/base/config/float.h
    include/base/config/noexcept_release.h
    include/base/erase_remove_if.h
    include/base/fast_pimpl.h
    include/base/ignore_unused.h
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)
