project(physics
        LANGUAGES CXX)

set(HEADERS
    include/physics/consts/boltzmann_constant.h
    include/physics/consts/planck_constant.h
    include/physics/consts/speed_of_light.h
)

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})

target_include_directories(${PROJECT_NAME}
  INTERFACE include)

#target_link_libraries(
#  PRIVATE base)
