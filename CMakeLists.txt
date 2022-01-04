cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)

project(CMlyst VERSION 0.6.0 LANGUAGES CXX)

include(GNUInstallDirs)

find_package(Qt5 5.10 COMPONENTS
    Core
    Network
    Sql
)
find_package(Cutelyst3Qt5 3.1.0 REQUIRED)
find_package(Cutelee6Qt5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMLYST_APP_DIR "${LIB_INSTALL_DIR}/cutelyst-apps" CACHE PATH "Installation directory for the Cutelyst app")
set(CMLYST_HTML_DIR "${SHARE_INSTALL_PREFIX}/cmlyst" CACHE PATH "Target directory for the Cutelee/HTML files")

# Build time config definitions
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)

file(GLOB_RECURSE TEMPLATES_SRC root/*)

install(DIRECTORY root DESTINATION ${CMLYST_HTML_DIR})

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

set(CPACK_PACKAGE_VENDOR "Cutelyst")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMlyst.")
set(CPACK_PACKAGE_CONTACT "dantti12@gmail.com.br")

if(UNIX)
  if(NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR "DEB")
  endif()

  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set(CPACK_STRIP_FILES 1)
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
  endif()
endif()

include(CPack)
