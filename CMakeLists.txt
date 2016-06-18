project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra \
 \ -Wno-unused-parameter -pedantic -g")

if (SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak -fsanitize=address")
endif ()

# we depend on Box2D, SFML, and enet, from submodules in thirdparty/
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/enet")
add_subdirectory("thirdparty/SFML")

# we also depend on boost and gthread from the host system
find_package(Boost REQUIRED)

# so we can #include stuff
include_directories(
        src/
        thirdparty/Box2D/Box2D/
        thirdparty/enet/include/
        thirdparty/cereal/
        thirdparty/SFML/include/
        thirdparty/polypartition/
        thirdparty/mingw-std-threads/
        thirdparty/miniz/
        "${Boost_INCLUDE_DIRS}"
)

###### libsolemnsky, for common use by client and server
add_library(solemnsky STATIC
        src/sky/environment/envgraphics.cpp
        src/sky/environment/envgraphics.hpp

        src/sky/environment/environment.cpp
        src/sky/environment/environment.hpp

        src/sky/environment/envscripts.cpp
        src/sky/environment/envscripts.hpp

        src/sky/environment/map.cpp
        src/sky/environment/map.hpp

        src/sky/arena.cpp
        src/sky/arena.hpp

        src/sky/debugview.cpp
        src/sky/debugview.hpp

        src/sky/event.cpp
        src/sky/event.hpp

        src/sky/participation.cpp
        src/sky/participation.hpp

        src/sky/physics.cpp
        src/sky/physics.hpp

        src/sky/planestate.cpp
        src/sky/planestate.hpp

        src/sky/prop.cpp
        src/sky/prop.hpp

        src/sky/protocol.cpp
        src/sky/protocol.hpp

        src/sky/scoreboard.cpp
        src/sky/scoreboard.hpp

        src/sky/sky.cpp
        src/sky/sky.hpp

        src/sky/skyhandle.cpp
        src/sky/skyhandle.hpp

        src/sky/skysettings.cpp
        src/sky/skysettings.hpp

        src/util/methods.cpp
        src/util/methods.hpp

        src/util/printer.cpp
        src/util/printer.hpp

        src/util/telegraph.cpp
        src/util/telegraph.hpp

        src/util/threads.hpp

        src/util/types.cpp
        src/util/types.hpp

        thirdparty/polypartition/polypartition.cpp
        thirdparty/polypartition/polypartition.hpp
        )

target_link_libraries(solemnsky
        enet
        sfml-system
        sfml-network
        sfml-graphics
        Box2D
        ${Boost_LIBRARIES}
        )

###### solemnsky_server
add_executable(solemnsky_server
        src/server/servers/vanilla.cpp
        src/server/servers/vanilla.hpp

        src/server/latencytracker.cpp
        src/server/latencytracker.hpp

        src/server/main.cpp

        src/server/server.cpp
        src/server/server.hpp
        )
target_link_libraries(solemnsky_server
        solemnsky
        )

###### solemnsky_client
add_executable(solemnsky_client
        src/client/elements/clientshared.cpp
        src/client/elements/clientshared.hpp

        src/client/elements/elements.cpp
        src/client/elements/elements.hpp

        src/client/elements/settings.cpp
        src/client/elements/settings.hpp

        src/client/elements/style.cpp
        src/client/elements/style.hpp

        src/client/multiplayer/multiplayer.cpp
        src/client/multiplayer/multiplayer.hpp

        src/client/multiplayer/multiplayercore.cpp
        src/client/multiplayer/multiplayercore.hpp

        src/client/multiplayer/multiplayergame.cpp
        src/client/multiplayer/multiplayergame.hpp

        src/client/multiplayer/multiplayerlobby.cpp
        src/client/multiplayer/multiplayerlobby.hpp

        src/client/multiplayer/multiplayerscoring.cpp
        src/client/multiplayer/multiplayerscoring.hpp

        src/client/sandbox/sandbox.cpp
        src/client/sandbox/sandbox.hpp

        src/client/client.cpp
        src/client/client.hpp

        src/client/homepage.cpp
        src/client/homepage.hpp

        src/client/listingpage.cpp
        src/client/listingpage.hpp

        src/client/settingspage.cpp
        src/client/settingspage.hpp

        src/client/main.cpp

        src/client/skyrender.cpp
        src/client/skyrender.hpp

        src/ui/widgets/button.cpp
        src/ui/widgets/button.hpp

        src/ui/widgets/checkbox.cpp
        src/ui/widgets/checkbox.hpp

        src/ui/widgets/keyselector.cpp
        src/ui/widgets/keyselector.hpp

        src/ui/widgets/textentry.cpp
        src/ui/widgets/textentry.hpp

        src/ui/widgets/textlog.cpp
        src/ui/widgets/textlog.hpp

        src/ui/control.cpp
        src/ui/control.hpp

        src/ui/frame.cpp
        src/ui/frame.hpp

        src/ui/resources.cpp
        src/ui/resources.hpp

        src/ui/sheet.cpp
        src/ui/sheet.hpp

        src/ui/text.cpp
        src/ui/text.hpp

        src/ui/splash.cpp
        src/ui/splash.hpp

        src/ui/widgets.hpp

        src/util/clientutil.cpp
        src/util/clientutil.hpp
        )
target_link_libraries(solemnsky_client
        solemnsky
        sfml-window
        )

###### unit tests
add_subdirectory(tests/)

###### installation
install(TARGETS solemnsky_client solemnsky_server
        RUNTIME DESTINATION bin)
install(DIRECTORY media DESTINATION share/solemnsky)
set(CPACK_GENERATOR "ZIP")
include(CPack)

