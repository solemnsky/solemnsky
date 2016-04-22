project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic")

# we depend on Box2D, SFML, and enet, from submodules in thirdparty/
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/SFML")
add_subdirectory("thirdparty/enet")

# we also depend on boost and gthread from the host system
find_package(Boost)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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

        src/sky/prop.cpp
        src/sky/prop.h

        src/sky/protocol.cpp
        src/sky/protocol.h

        src/sky/sky.cpp
        src/sky/sky.h

        src/util/methods.cpp
        src/util/methods.h

        src/util/printer.cpp
        src/util/printer.h

        src/util/telegraph.cpp
        src/util/telegraph.h

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
        src/server/servers/vanilla.cpp
        src/server/servers/vanilla.h

        src/server/main.cpp

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

        src/client/tutorial/tutorial.cpp
        src/client/tutorial/tutorial.h

        src/client/client.cpp
        src/client/client.h

        src/client/homepage.cpp
        src/client/homepage.h

        src/client/listingpage.cpp
        src/client/listingpage.h

        src/client/settingspage.cpp
        src/client/settingspage.h

        src/client/main.cpp

        src/client/skyrender.cpp
        src/client/skyrender.h

        src/ui/widgets/button.cpp
        src/ui/widgets/button.h

        src/ui/widgets/checkbox.cpp
        src/ui/widgets/checkbox.h

        src/ui/widgets/keyselector.cpp
        src/ui/widgets/keyselector.h

        src/ui/widgets/textentry.cpp
        src/ui/widgets/textentry.h

        src/ui/widgets/textlog.cpp
        src/ui/widgets/textlog.h

        src/ui/widgets/widgets.h

        src/ui/control.cpp
        src/ui/control.h

        src/ui/frame.cpp
        src/ui/frame.h

        src/ui/sheet.cpp
        src/ui/sheet.h

        src/ui/text.cpp
        src/ui/text.h

        src/ui/wrapper.cpp
        src/ui/wrapper.h

        src/util/client/resources.cpp
        src/util/client/resources.h

        src/util/client/sfmlutil.cpp
        src/util/client/sfmlutil.h
        )
target_link_libraries(solemnsky_client
        solemnsky
        sfml-graphics
        sfml-window
        ${CMAKE_THREAD_LIBS_INIT}
        )

###### unit tests
add_subdirectory(tests/)

###### cpack configuration
SET(CPACK_GENERATOR "ZIP")
include(CPack)
