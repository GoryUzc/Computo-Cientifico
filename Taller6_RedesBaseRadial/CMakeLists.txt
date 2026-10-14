cmake_minimum_required(VERSION 3.16)
project(Taller6_RBF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_APPLOCAL_DEPS OFF)
find_package(Eigen3 CONFIG REQUIRED)

# Ejecutable principal
add_executable(taller6_rbf
    src/main.cpp
    src/generador_datos.cpp
    src/rbf_core.cpp
    src/evaluador.cpp
    src/exportador.cpp
)

# Inclusiones y enlaces
target_include_directories(taller6_rbf PRIVATE src)
target_link_libraries(taller6_rbf PRIVATE Eigen3::Eigen)

# Optimizaciones agresivas para cálculo numérico
if(MSVC)
    target_compile_options(taller6_rbf PRIVATE /W4 /O2 /fp:fast)
else()
    target_compile_options(taller6_rbf PRIVATE -Wall -Wextra -O3 -ffast-math -march=native)
endif()
