cmake_minimum_required(VERSION 3.16)
project(SvdImageCompression LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain (expects VCPKG_ROOT env var or pass via command line)
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}" CACHE PATH "vcpkg root directory")
if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Eigen comes from vcpkg (triplet x64-windows) — no min version; use imported target
find_package(Eigen3 REQUIRED)

add_executable(svd_compresion
    src/main.cpp
    src/cargador_imagen.cpp
    src/svd_procesador.cpp
    src/metricas.cpp
    src/exportador.cpp
)

target_include_directories(svd_compresion PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link imported Eigen3::Eigen target — propagates includes transitively
target_link_libraries(svd_compresion PRIVATE Eigen3::Eigen)

target_compile_options(svd_compresion PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Wall>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O2>
)

# ── Test unitario: reconstrucción exacta k = rango (SPEC-000 §12 #3) ──
add_executable(test_reconstruccion_exacta
    src/test_reconstruccion_exacta.cpp
)

target_include_directories(test_reconstruccion_exacta PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_reconstruccion_exacta PRIVATE Eigen3::Eigen)

target_compile_options(test_reconstruccion_exacta PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Wall>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O2>
)
