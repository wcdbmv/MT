cmake_minimum_required(VERSION 3.25)
project(physics
        LANGUAGES CXX)

set(HEADERS
    include/physics/consts/boltzmann_constant.h
    include/physics/consts/planck_constant.h
    include/physics/consts/speed_of_light.h
    include/physics/params.h
    include/physics/plancks_law.h
)

add_library(${PROJECT_NAME} INTERFACE
            ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)

target_link_libraries(${PROJECT_NAME}
                      INTERFACE base math)
