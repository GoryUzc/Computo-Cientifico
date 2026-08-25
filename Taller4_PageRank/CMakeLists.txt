cmake_minimum_required(VERSION 3.16)
project(PageRank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain (opcional): define VCPKG_ROOT o pásalo con
# -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}" CACHE PATH "vcpkg root directory")
if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Eigen3
find_package(Eigen3 REQUIRED)

# Ejecutable
add_executable(pagerank
    src/main.cpp
    src/pagerank_core.cpp
    src/casos_prueba.cpp
    src/exportador.cpp
)

target_link_libraries(pagerank PRIVATE Eigen3::Eigen)
target_include_directories(pagerank PRIVATE src)

# Optimizaciones
target_compile_options(pagerank PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O2>
)
