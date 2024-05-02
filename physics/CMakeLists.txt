project(physics
        LANGUAGES CXX)

set(HEADERS
    include/physics/consts/boltzmann_constant.h
    include/physics/consts/planck_constant.h
    include/physics/consts/speed_of_light.h
    include/physics/reflect.h
    include/physics/refract.h
)

set(SOURCES
    src/reflect.cc
    src/refract.cc
)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)

target_link_libraries(${PROJECT_NAME}
  PRIVATE base math)
