project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# source files for executable
include_directories(src/) # so we can #include "directory/something.h"
set(SOURCE_FILES src/main.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/base/control.h src/base/control.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/base/profiler.h src/base/profiler.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/base/util.h src/base/util.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/engine/engine.h src/engine/engine.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/engine/flight.h src/engine/flight.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/engine/physics.h src/engine/physics.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/demo/graphics.h src/demo/graphics.cpp)

add_executable(solemnsky ${SOURCE_FILES})

# libs: SFML 2.2 and box2d 2.3
include_directories(/home/magneticduck/.nix-profile/include/)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libsfml-system.so)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libsfml-graphics.so)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libsfml-window.so)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libBox2D.so)
