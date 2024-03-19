cmake_minimum_required(VERSION 3.25)
project(geogebra
        LANGUAGES CXX)

set(HEADERS
    include/geogebra/api.h
)

set(SOURCES
    src/api.cc
)

add_library(${PROJECT_NAME} STATIC
            ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE base math)
