project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# we depend on Box2D, SFML, and enet, from submodules in thirdparty/
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/SFML")
add_subdirectory("thirdparty/enet")

# we also depend on boost from the host system
find_package(Boost)

# so we can #include stuff
include_directories(
        src/
        thirdparty/SFML/include/
        thirdparty/Box2D/Box2D/
        thirdparty/enet/include/
        #        thirdparty/enetpp/include/
        "${Boost_INCLUDE_DIRS}"
)

###### libsolemnsky, for common use by client and server
add_library(solemnsky STATIC
        src/sky/arena.cpp
        src/sky/arena.h

        src/sky/flight.cpp
        src/sky/flight.h

        src/sky/map.cpp
        src/sky/map.h

        src/sky/physics.cpp
        src/sky/physics.h

        src/sky/protocol.cpp
        src/sky/protocol.h

        src/sky/sky.cpp
        src/sky/sky.h

        src/telegraph/pack.cpp
        src/telegraph/pack.h

        src/telegraph/packet.cpp
        src/telegraph/packet.h

        src/telegraph/telegraph.cpp
        src/telegraph/telegraph.h

        src/util/methods.cpp
        src/util/methods.h

        src/util/types.cpp
        src/util/types.h
        )
target_link_libraries(solemnsky
        enet
        sfml-system
        sfml-network
        Box2D_shared
        ${Boost_LIBRARIES}
        )

###### solemnsky_server
add_executable(solemnsky_server
        src/server/subsystem/serverlog.cpp
        src/server/subsystem/serverlog.h

        src/server/server.cpp
        src/server/server.h
        )
target_link_libraries(solemnsky_server
        solemnsky
        )

###### solemnsky_client
add_executable(solemnsky_client
        src/client/elements/clientshared.cpp
        src/client/elements/clientshared.h

        src/client/elements/elements.cpp
        src/client/elements/elements.h

        src/client/elements/settings.cpp
        src/client/elements/settings.h

        src/client/game/gamecontrols.cpp
        src/client/game/gamecontrols.h

        src/client/game/multiplayerclient.cpp
        src/client/game/multiplayerclient.h

        src/client/game/tutorial.cpp
        src/client/game/tutorial.h

        src/client/subsystem/render.cpp
        src/client/subsystem/render.h

        src/client/ui/widgets/button.cpp
        src/client/ui/widgets/button.h

        src/client/ui/widgets/radiobutton.cpp
        src/client/ui/widgets/radiobutton.h

        src/client/ui/widgets/textentry.cpp
        src/client/ui/widgets/textentry.h

        src/client/ui/widgets/textlog.cpp
        src/client/ui/widgets/textlog.h

        src/client/ui/control.cpp
        src/client/ui/control.h

        src/client/ui/frame.cpp
        src/client/ui/frame.h

        src/client/ui/profiler.cpp
        src/client/ui/profiler.h

        src/client/ui/sheet.cpp
        src/client/ui/sheet.h

        src/client/ui/splash.cpp
        src/client/ui/splash.h

        src/client/util/resources.cpp
        src/client/util/resources.h

        src/client/util/sfmlutil.cpp
        src/client/util/sfmlutil.h

        src/client/client.cpp
        src/client/client.h

        src/client/homepage.cpp
        src/client/homepage.h

        src/client/listingpage.cpp
        src/client/listingpage.h

        src/client/settingspage.cpp
        src/client/settingspage.h
        )
target_link_libraries(solemnsky_client
        solemnsky
        sfml-graphics
        sfml-window
        )

###### unit tests
add_subdirectory(tests/)
