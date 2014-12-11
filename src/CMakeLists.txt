include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Our sources
set(untitled_SRCS
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
    adminposts.cpp
    blog.cpp
    untitled.cpp
)

# C++11 rocks!
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# Create the application
add_library(untitled SHARED ${untitled_SRCS})

# Link to Qt modules
qt5_use_modules(untitled Core Network)

# Link to Cutelyst
target_link_libraries(untitled
    ${CutelystQt5_LIBRARY}
)

# TODO install to a place where uWSGI Cutelyst plugin searches for
install(TARGETS untitled DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

#add_executable(untitled-app  ${untitled_SRCS} main.cpp)
#qt5_use_modules(untitled-app Core Network Sql)
#target_link_libraries(untitled-app
#    ${CutelystQt5_LIBRARY}
#)
