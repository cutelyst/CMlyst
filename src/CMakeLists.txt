include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Our sources
set(cmlyst_SRCS
    ${TEMPLATES_SRC}
    ../libCMS/page.cpp
    ../libCMS/page_p.h
    ../libCMS/engine.cpp
    ../libCMS/engine_p.h
    ../libCMS/fileengine.cpp
    ../libCMS/fileengine_p.h
    root.cpp
    admin.cpp
    adminsetup.cpp
    adminlogin.cpp
    adminappearance.cpp
    adminposts.cpp
    adminpages.cpp
    adminmedia.cpp
    adminsettings.cpp
    blog.cpp
    cmlyst.cpp
)

# C++11 rocks!
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# Create the application
add_library(cmlyst SHARED ${cmlyst_SRCS})

# Link to Qt modules
qt5_use_modules(cmlyst Core Network)

# Link to Cutelyst
target_link_libraries(cmlyst
    ${CutelystQt5_LIBRARY}
)

# TODO install to a place where uWSGI Cutelyst plugin searches for
install(TARGETS cmlyst DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
