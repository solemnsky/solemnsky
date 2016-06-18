add_subdirectory(googletest/)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(solemnsky_tests
        arenatest.cpp
        environmenttest.cpp
        protocoltest.cpp
        scoreboardtest.cpp
        skyhandletest.cpp
        skytest.cpp
        subsystemtest.cpp
        telegraphtest.cpp
        threadtest.cpp
        utiltest.cpp
        )
target_link_libraries(solemnsky_tests
        gtest
        gtest_main
        solemnsky
        )
install(TARGETS solemnsky_tests RUNTIME DESTINATION bin)
