cmake_minimum_required(VERSION 3.25)
project(fibonacci_sphere
        LANGUAGES CXX)

find_package(Matplot++ REQUIRED)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE Matplot++::matplot math)
