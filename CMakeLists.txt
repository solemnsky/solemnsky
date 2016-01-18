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
add_library(solemnsky 
        src/base/base.h

        src/base/sysutil.cpp
        src/base/sysutil.h

        src/base/value.cpp
        src/base/value.h

        src/sky/delta.cpp
        src/sky/delta.h

        src/sky/flight.h
        src/sky/flight.cpp

        src/sky/physics.h
        src/sky/physics.cpp

        src/sky/sky.h
        src/sky/sky.cpp

        src/sky/subsystem.h
        )
target_link_libraries(solemnsky
        sfml-system
        Box2D_shared
        ${Boost_LIBRARIES})

###### solemnsky_server
add_executable(solemnsky_server
        src/sky/server/deltacom.cpp
        src/sky/server/deltacom.h

        src/server/server.h
        src/solemnsky_server.cpp)
target_link_libraries(solemnsky_server
        solemnsky)

###### solemnsky_client
add_executable(solemnsky_client
        src/base/resources.cpp
        src/base/resources.h

        src/base/util.cpp
        src/base/util.h

        src/client/client.cpp
        src/client/client.h

        src/client/gamecontroller.cpp
        src/client/gamecontroller.h

        src/client/tabselector.cpp
        src/client/tabselector.h

        src/sky/client/render.cpp
        src/sky/client/render.h

        src/ui/elements/button.cpp
        src/ui/elements/button.h

        src/ui/elements/splash.cpp
        src/ui/elements/splash.h

        src/ui/control.h
        src/ui/control.cpp

        src/ui/frame.cpp
        src/ui/frame.h

        src/ui/profiler.h
        src/ui/profiler.cpp

        src/ui/sheet.cpp
        src/ui/sheet.h

        src/ui/signal.h

        src/ui/ui.h

        src/solemnsky_client.cpp)
target_link_libraries(solemnsky_client
        solemnsky
        sfml-graphics
        sfml-window)
