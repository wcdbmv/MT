cmake_minimum_required(VERSION 3.25)
project(base
        LANGUAGES CXX)

set(HEADERS
    include/base/erase_remove_if.h
    include/base/float.h
    include/base/float_cmp.h
    include/base/noexcept_release.h
)

add_library(${PROJECT_NAME} INTERFACE
            ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)
