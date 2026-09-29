cmake_minimum_required(VERSION 3.16)
project(SEIR_RK4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen3
set(VCPKG_APPLOCAL_DEPS OFF)
find_package(Eigen3 CONFIG REQUIRED)

# Ejecutable
add_executable(seir_rk4
    src/main.cpp
    src/modelo_seir.cpp
    src/solver_rk4.cpp
    src/exportador.cpp
)

target_link_libraries(seir_rk4 PRIVATE Eigen3::Eigen)
target_include_directories(seir_rk4 PRIVATE src)