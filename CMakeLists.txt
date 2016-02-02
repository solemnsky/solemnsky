project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# we depend on Box2D and SFML, which we package ourselves
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/SFML")

# we also depend on boost from the host system
find_package(Boost)

# so we can #include stuff
include_directories(
        src/
        thirdparty/SFML/include/
        thirdparty/Box2D/Box2D/
        "${Boost_INCLUDE_DIRS}"
)

###### libsolemnsky, for common use by client and server
add_library(solemnsky STATIC
        src/pack/packet.cpp
        src/pack/packet.h

        src/util/methods.cpp
        src/util/methods.h

        src/util/types.cpp
        src/util/types.h

        src/sky/delta.cpp
        src/sky/delta.h

        src/sky/flight.h
        src/sky/flight.cpp

        src/sky/map.cpp
        src/sky/map.h

        src/sky/physics.h
        src/sky/physics.cpp

        src/sky/sky.h
        src/sky/sky.cpp

        src/sky/subsystem.h
        src/pack/pack.cpp src/pack/pack.h)
target_link_libraries(solemnsky
        sfml-system
        sfml-network
        Box2D_shared
        ${Boost_LIBRARIES}
        )

###### solemnsky_server
add_executable(solemnsky_server
        src/server/subsystem/deltacom.cpp
        src/server/subsystem/deltacom.h

        src/server/server.cpp
        src/server/server.h
        )
target_link_libraries(solemnsky_server
        solemnsky
        )

###### solemnsky_client
add_executable(solemnsky_client
        src/client/game/game.h

        src/client/game/gamecontroller.cpp
        src/client/game/gamecontroller.h

        src/client/game/tutorial.cpp
        src/client/game/tutorial.h

        src/client/ui/elements/button.cpp
        src/client/ui/elements/button.h

        src/client/ui/elements/splash.cpp
        src/client/ui/elements/splash.h

        src/client/ui/control.h
        src/client/ui/control.cpp

        src/client/ui/frame.cpp
        src/client/ui/frame.h

        src/client/ui/profiler.h
        src/client/ui/profiler.cpp

        src/client/ui/sheet.cpp
        src/client/ui/sheet.h

        src/client/ui/signal.h

        src/client/util/resources.cpp
        src/client/util/resources.h

        src/client/util/sfmlutil.cpp
        src/client/util/sfmlutil.h

        src/client/client.cpp
        src/client/client.h

        src/client/clientstate.cpp
        src/client/clientstate.h

        src/client/homepage.cpp
        src/client/homepage.h

        src/client/listingpage.cpp
        src/client/listingpage.h

        src/client/page.h

        src/client/settingspage.cpp
        src/client/settingspage.h

        src/client/subsystem/render.cpp
        src/client/subsystem/render.h
        )
target_link_libraries(solemnsky_client
        solemnsky
        sfml-graphics
        sfml-window
        )
