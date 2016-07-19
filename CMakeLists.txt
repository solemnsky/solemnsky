project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CAREFUL_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if (SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak -fsanitize=address -g")
endif ()

# thirdparty submodules that need to be built
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/enet")
add_subdirectory("thirdparty/SFML")

# we also depend on boost and ibarchive from the host system
find_package(Boost REQUIRED filesystem)

# project includes
include_directories(src/)

## thirdparty includes, with -isystem to disable warnings
include_directories(SYSTEM
        thirdparty/Box2D/Box2D/
        thirdparty/enet/include/
        thirdparty/cereal/
        thirdparty/SFML/include/
        thirdparty/polypartition/
        thirdparty/mingw-std-threads/
        thirdparty/spdlog/include/
        ${Boost_INCLUDE_DIRS}
        )

###### libsolemnsky, for common use by client and server
add_library(solemnsky STATIC
        src/engine/environment/environment.cpp
        src/engine/environment/environment.hpp

        src/engine/environment/map.cpp
        src/engine/environment/map.hpp

        src/engine/environment/mechanics.cpp
        src/engine/environment/mechanics.hpp

        src/engine/environment/visuals.cpp
        src/engine/environment/visuals.hpp

        src/engine/sky/participation.cpp
        src/engine/sky/participation.hpp

        src/engine/sky/physics.cpp
        src/engine/sky/physics.hpp

        src/engine/sky/planestate.cpp
        src/engine/sky/planestate.hpp

        src/engine/sky/prop.cpp
        src/engine/sky/prop.hpp

        src/engine/sky/sky.cpp
        src/engine/sky/sky.hpp

        src/engine/sky/skyhandle.cpp
        src/engine/sky/skyhandle.hpp

        src/engine/sky/skysettings.cpp
        src/engine/sky/skysettings.hpp

        src/engine/arena.cpp
        src/engine/arena.hpp

        src/engine/debugview.cpp
        src/engine/debugview.hpp

        src/engine/event.cpp
        src/engine/event.hpp

        src/engine/player.cpp
        src/engine/player.hpp

        src/engine/protocol.cpp
        src/engine/protocol.hpp

        src/engine/scoreboard.cpp
        src/engine/scoreboard.hpp

        src/engine/types.cpp
        src/engine/types.hpp

        src/util/archive.cpp
        src/util/archive.hpp

        src/util/filepath.cpp
        src/util/filepath.hpp

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
set_target_properties(solemnsky PROPERTIES COMPILE_FLAGS "${CAREFUL_CXX_FLAGS}")

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
set_target_properties(solemnsky_server PROPERTIES COMPILE_FLAGS "${CAREFUL_CXX_FLAGS}")

###### solemnsky_client
add_executable(solemnsky_client
        src/client/elements/clientshared.cpp
        src/client/elements/clientshared.hpp

        src/client/elements/clientui.cpp
        src/client/elements/clientui.hpp

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

        src/client/multiplayer/multiplayergamehandle.cpp
        src/client/multiplayer/multiplayergamehandle.hpp

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

        src/ui/widgets/transformed.cpp
        src/ui/widgets/transformed.hpp

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
set_target_properties(solemnsky_client PROPERTIES COMPILE_FLAGS "${CAREFUL_CXX_FLAGS}")

###### unit tests
add_subdirectory(tests/)

# Group the source files together so they appear nicely in Xcode
source_group("engine\\environment" REGULAR_EXPRESSION src/engine/environment/.*)
source_group("engine"              REGULAR_EXPRESSION src/engine/.*)
source_group("engine\\sky"         REGULAR_EXPRESSION src/engine/sky/.*)
source_group("util"                REGULAR_EXPRESSION src/util/.*)
source_group("server\\servers"     REGULAR_EXPRESSION src/server/servers/.*)
source_group("server"              REGULAR_EXPRESSION src/server/.*)
source_group("client\\elements"    REGULAR_EXPRESSION src/client/elements/.*)
source_group("client\\multiplayer" REGULAR_EXPRESSION src/client/multiplayer/.*)
source_group("client\\sandbox"     REGULAR_EXPRESSION src/client/sandbox/.*)
source_group("client"              REGULAR_EXPRESSION src/client/.*)
source_group("ui\\widgets"         REGULAR_EXPRESSION src/ui/widgets/.*)
source_group("ui"                  REGULAR_EXPRESSION src/ui/.*)
source_group("thirdparty"          REGULAR_EXPRESSION thirdparty/.*)

###### installation
install(TARGETS solemnsky_client solemnsky_server
        RUNTIME DESTINATION bin)
install(DIRECTORY media DESTINATION share/solemnsky)
set(CPACK_GENERATOR "ZIP")
include(CPack)

