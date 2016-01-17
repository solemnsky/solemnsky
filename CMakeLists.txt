project(solemnsky)
project(solemnsky-srv)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Third party Libraries
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/SFML")

# source files for common library
set(LIB_FILES
        src/base/base.h

        src/base/resources.cpp
        src/base/resources.h

        src/base/util.cpp
        src/base/util.h

        src/base/value.cpp
        src/base/value.h

        src/client/demo/demo.cpp
        src/client/demo/demo.h

        src/client/demo/graphicsdemo.cpp
        src/client/demo/graphicsdemo.h

        src/client/demo/physdemo.cpp
        src/client/demo/physdemo.h

        src/client/demo/skydemo.cpp
        src/client/demo/skydemo.h

        src/client/client.cpp
        src/client/client.h

        src/server/server.cpp
        src/server/server.h

        src/sky/client/render.cpp
        src/sky/client/render.h

        src/sky/server/deltacom.cpp
        src/sky/server/deltacom.h

        src/sky/delta.cpp
        src/sky/delta.h

        src/sky/flight.h
        src/sky/flight.cpp

        src/sky/physics.h
        src/sky/physics.cpp

        src/sky/sky.h
        src/sky/sky.cpp

        src/sky/subsystem.h

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
        )

find_package(Boost)

# Libraries for the client
set(LIBRARIES_CLIENT
        # libs: SFML 2.2 and box2d 2.3
        sfml-system
        sfml-graphics
        sfml-window
        Box2D
        "${Boost_LIBRARIES}"
        )

# Libraries for the server
set(LIBRARIES_SERVER
        # libs: SFML 2.2 non-graphical and box2d 2.3
        sfml-system
        Box2D
        "${Boost_LIBRARIES}"
        )

# Paths to headers for inclusion
set(INCLUDE_DIRS
        # so we can #include "directory/something.h"
        src/
        thirdparty/SFML/include/
        thirdparty/Box2D/Box2D/
        "${Boost_INCLUDE_DIRS}"
        )

add_executable(solemnsky ${LIB_FILES} src/main.cpp)
add_executable(solemnsky-srv ${LIB_FILES} src/servmain.cpp)

include_directories(${INCLUDE_DIRS})
target_link_libraries(solemnsky ${LIBRARIES_CLIENT})
target_link_libraries(solemnsky-srv ${LIBRARIES_SERVER})

