project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Third party Libraries
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/SFML")

# source files for executable
set(SOURCE_FILES
        src/main.cpp

        src/base/base.h

        src/base/value.cpp
        src/base/value.h

        src/base/util.h
        src/base/util.cpp

        src/base/resources.cpp
        src/base/resources.h

        src/demo/demo.h
        src/demo/demo.cpp

        src/demo/graphicsdemo.h
        src/demo/graphicsdemo.cpp

        src/demo/physdemo.cpp
        src/demo/physdemo.h

        src/demo/skydemo.cpp
        src/demo/skydemo.h

        src/sky/sky.h
        src/sky/sky.cpp

        src/sky/flight.h
        src/sky/flight.cpp

        src/sky/render.cpp
        src/sky/render.h

        src/sky/physics.h
        src/sky/physics.cpp

        src/ui/elements/button.cpp
        src/ui/elements/button.h

        src/ui/control.h
        src/ui/control.cpp

        src/ui/frame.cpp
        src/ui/frame.h

        src/ui/profiler.h
        src/ui/profiler.cpp

        src/ui/sheet.cpp
        src/ui/sheet.h

        src/ui/signal.cpp
        src/ui/signal.h

        src/ui/ui.h
        src/base/base.h
        src/ui/elements/splash.cpp src/ui/elements/splash.h)


find_package(Boost)

# Libraries
set(LIBRARIES
        # libs: SFML 2.2 and box2d 2.3
        sfml-system
        sfml-graphics
        sfml-window
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

add_executable(solemnsky ${SOURCE_FILES})
include_directories(${INCLUDE_DIRS})
target_link_libraries(solemnsky ${LIBRARIES})
