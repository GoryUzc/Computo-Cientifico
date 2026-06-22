cmake_minimum_required(VERSION 3.16)
project(Calor2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)

add_executable(calor_2d
    src/main.cpp
    src/malla.cpp
    src/ensamblador.cpp
    src/solver.cpp
    src/exportador.cpp
)

target_link_libraries(calor_2d Eigen3::Eigen)