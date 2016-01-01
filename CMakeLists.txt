project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Third party Libraries
add_subdirectory("thirdparty/Box2D/Box2D")
add_subdirectory("thirdparty/SFML")

# source files for executable
set(SOURCE_FILES
    src/main.cpp

    src/base/control.h
    src/base/control.cpp

    src/base/physics.h
    src/base/physics.cpp

    src/base/profiler.h
    src/base/profiler.cpp

    src/base/util.h
    src/base/util.cpp

    src/demo/demo.h
    src/demo/demo.cpp

    src/demo/graphicsdemo.h
    src/demo/graphicsdemo.cpp

    src/engine/engine.h
    src/engine/engine.cpp

    src/engine/flight.h
    src/engine/flight.cpp

    src/engine/graphics.cpp
    src/engine/graphics.h

    src/demo/enginedemo.cpp
    src/demo/enginedemo.h

    src/base/ui/button.cpp
    src/base/ui/button.h

    src/base/ui.h)

#find_package(Boost)

# Libraries
set(LIBRARIES
    # libs: SFML 2.2 and box2d 2.3
    sfml-system
    sfml-graphics
    sfml-window
    Box2D
    #  "${Boost_LIBRARIES}"
    )

# Paths to headers for inclusion
set(INCLUDE_DIRS
    # so we can #include "directory/something.h"
    src/
    thirdparty/SFML/include/
    thirdparty/Box2D/Box2D/
    #  "${Boost_INCLUDE_DIRS}"
    )

add_executable(solemnsky ${SOURCE_FILES})
include_directories(${INCLUDE_DIRS})
target_link_libraries(solemnsky ${LIBRARIES})
