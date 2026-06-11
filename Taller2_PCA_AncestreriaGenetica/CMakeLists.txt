cmake_minimum_required(VERSION 3.16)
project(pca_genetica LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen3
find_package(Eigen3 CONFIG REQUIRED)

# Source files
set(SRC_FILES
    src/cargador_csv.cpp
    src/procesador_pca.cpp
    src/svd_calculador.cpp
    src/exportador.cpp
    src/main.cpp
)

add_executable(pca_genetica ${SRC_FILES})


target_include_directories(pca_genetica PRIVATE 
    src
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # Cambia "external" por el nombre real de la carpeta
)


target_link_libraries(pca_genetica PRIVATE Eigen3::Eigen)

# Compiler warnings
if(MSVC)
    target_compile_options(pca_genetica PRIVATE /W4)
else()
    target_compile_options(pca_genetica PRIVATE -Wall -Wextra)
endif()
