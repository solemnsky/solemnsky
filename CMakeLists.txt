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

  src/base/control.h
  src/base/control.cpp

  src/base/profiler.h
  src/base/profiler.cpp

  src/base/util.h
  src/base/util.cpp

  src/engine/engine.h
  src/engine/engine.cpp

  src/engine/flight.h
  src/engine/flight.cpp

  src/engine/physics.h
  src/engine/physics.cpp

  src/demo/graphics.h
  src/demo/graphics.cpp

  src/demo/demo.h
  src/demo/demo.cpp
)

# Libraries
SET (LIBRARIES
  # libs: SFML 2.2 and box2d 2.3
  sfml-system
  sfml-graphics
  sfml-window
  Box2D
)

# Paths to headers for inclusion
SET (INCLUDE_DIRS
  # so we can #include "directory/something.h"
  src/
  thirdparty/SFML/include/
  thirdparty/Box2D/Box2D/
)

add_executable(solemnsky ${SOURCE_FILES})
include_directories(${INCLUDE_DIRS})
target_link_libraries(solemnsky ${LIBRARIES})
