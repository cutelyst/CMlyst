project(CMlyst)

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

find_package(Qt5 COMPONENTS
    Core
    Network
    Sql
)
find_package(Cutelyst2Qt5 2.0.0 REQUIRED)
find_package(Grantlee5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMLYST_APP_DIR "${LIB_INSTALL_DIR}/cutelyst-apps" CACHE PATH "Installation directory for the Cutelyst app")
set(CMLYST_HTML_DIR "${SHARE_INSTALL_PREFIX}/cmlyst" CACHE PATH "Target directory for the Grantlee/HTML files")

# Build time config definitions
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)

file(GLOB_RECURSE TEMPLATES_SRC root/*)

install(DIRECTORY root DESTINATION ${CMLYST_HTML_DIR})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Cutelyst2Qt5_INCLUDE_DIR}
)

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
)

add_subdirectory(src)
