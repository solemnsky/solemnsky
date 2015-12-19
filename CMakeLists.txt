project(solemnsky)

# tool / compiler versions
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# source files for executable
include_directories(src/) # so we can #include "solemnsky/something/something"
set(SOURCE_FILES src/main.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/ctrl/ctrl.h src/ctrl/ctrl.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/ctrl/log.h src/ctrl/log.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/ctrl/profile.h src/ctrl/profile.cpp)
add_executable(solemnsky ${SOURCE_FILES})

# libs
include_directories(/home/magneticduck/.nix-profile/include/)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libsfml-system.so)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libsfml-graphics.so)
target_link_libraries(solemnsky /home/magneticduck/.nix-profile/lib/libsfml-window.so)

# TODO: make this work on other systems
