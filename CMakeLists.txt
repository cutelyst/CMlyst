project(CMlyst)

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

find_package(Qt5 COMPONENTS
    Core
    Network
    Sql
)
find_package(CutelystQt5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Build time config definitions
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)

file(GLOB_RECURSE TEMPLATES_SRC root/*)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CutelystQt5_INCLUDE_DIR}
)

add_subdirectory(src)
