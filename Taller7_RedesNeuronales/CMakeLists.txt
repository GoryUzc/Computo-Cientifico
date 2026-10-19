cmake_minimum_required(VERSION 3.10)
project(Backpropagation_NN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Eigen3
set(VCPKG_APPLOCAL_DEPS OFF)
find_package(Eigen3 CONFIG REQUIRED)

# Add executable
add_executable(backprop_nn
    src/main.cpp
    src/data_generator.cpp
    src/neural_network.cpp
    src/trainer.cpp
    src/exporter.cpp
)

# Include directories
target_include_directories(backprop_nn PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(backprop_nn PRIVATE Eigen3::Eigen)

# Create results directory at build time
add_custom_command(TARGET backprop_nn POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/resultados"
)