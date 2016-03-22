project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")

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
        thirdparty/cereal/
        "${Boost_INCLUDE_DIRS}"
)

###### libsolemnsky, for common use by client and server
add_library(solemnsky SHARED
        src/sky/arena.cpp
        src/sky/arena.h

        src/sky/event.cpp
        src/sky/event.h

        src/sky/map.cpp
        src/sky/map.h

        src/sky/physics.cpp
        src/sky/physics.h

        src/sky/plane.cpp
        src/sky/plane.h

        src/sky/props.cpp
        src/sky/props.h

        src/sky/protocol.cpp
        src/sky/protocol.h

        src/sky/sky.cpp
        src/sky/sky.h

        src/util/methods.cpp
        src/util/methods.h

        src/util/telegraph.cpp
        src/util/telegraph.h

        src/util/types.cpp
        src/util/types.h

        src/sky/subsystem.cpp
        src/sky/subsystem.h
        src/sky/subsystem.cpp src/sky/subsystem.h src/sky/networked.cpp src/sky/networked.h)
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

        src/client/elements/style.cpp
        src/client/elements/style.h

        src/client/multiplayer/multiplayer.cpp
        src/client/multiplayer/multiplayer.h

        src/client/multiplayer/multiplayergame.cpp
        src/client/multiplayer/multiplayergame.h

        src/client/multiplayer/multiplayerlobby.cpp
        src/client/multiplayer/multiplayerlobby.h

        src/client/multiplayer/multiplayerscoring.cpp
        src/client/multiplayer/multiplayerscoring.h

        src/client/multiplayer/multiplayershared.cpp
        src/client/multiplayer/multiplayershared.h

        src/client/subsystem/render.cpp
        src/client/subsystem/render.h

        src/client/tutorial/gamecontrols.cpp
        src/client/tutorial/gamecontrols.h

        src/client/tutorial/tutorial.cpp
        src/client/tutorial/tutorial.h

        src/client/ui/widgets/button.cpp
        src/client/ui/widgets/button.h

        src/client/ui/widgets/checkbox.cpp
        src/client/ui/widgets/checkbox.h

        src/client/ui/widgets/keyselector.cpp
        src/client/ui/widgets/keyselector.h

        src/client/ui/widgets/textentry.cpp
        src/client/ui/widgets/textentry.h

        src/client/ui/widgets/widgets.h

        src/client/ui/control.cpp
        src/client/ui/control.h

        src/client/ui/frame.cpp
        src/client/ui/frame.h

        src/client/ui/sheet.cpp
        src/client/ui/sheet.h

        src/client/ui/splash.cpp
        src/client/ui/splash.h

        src/client/ui/text.cpp
        src/client/ui/text.h

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

###### cpack configuration
SET(CPACK_GENERATOR "ZIP")
include(CPack)
