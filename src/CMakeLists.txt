# Our sources
set(cmlyst_SRCS
    ${TEMPLATES_SRC}
    libCMS/page.cpp
    libCMS/page_p.h
    libCMS/engine.cpp
    libCMS/engine_p.h
#    libCMS/fileengine.cpp
#    libCMS/fileengine_p.h
    libCMS/menu.cpp
    libCMS/menu_p.h
    libCMS/sqlengine.cpp
    sqluserstore.cpp
    cmengine.cpp
    cmdispatcher.cpp
    root.cpp
    admin.cpp
    adminsetup.cpp
    adminappearance.cpp
    adminpages.cpp
    adminposts.cpp
    adminmedia.cpp
    adminsettings.cpp
    cmlyst.cpp
    rsswriter.cpp
)

# Create the application
add_library(cmlyst SHARED ${cmlyst_SRCS})

# Link to Cutelyst
target_link_libraries(cmlyst
    Cutelyst::Core
    Cutelyst::View::Cutelee
    Cutelyst::Utils::Sql
    Cutelyst::Authentication
    Cutelyst::StatusMessage
    Cutelyst::Utils::Pagination
    Cutelyst::Session
    Cutelee5::Templates
    Qt5::Core
    Qt5::Network
    Qt5::Sql
)

install(TARGETS cmlyst
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
)

install(DIRECTORY ../root
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/cmlyst
)
