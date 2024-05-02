project(ray_tracing
        LANGUAGES CXX)

set(HEADERS
    include/ray_tracing/cylinder_z_infinite.h
    include/ray_tracing/elliptic_cylinder_z_infinite.h
    include/ray_tracing/shape.h
    include/ray_tracing/utils.h
)

set(SOURCES
    src/cylinder_z_infinite.cc
    src/elliptic_cylinder_z_infinite.cc
    src/shape.cc
    src/utils.cc
)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
  PRIVATE base math physics)

add_subdirectory(test)
