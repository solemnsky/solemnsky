add_subdirectory(googletest/)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(solemnsky_tests
        arenatest.cpp
        packtest.cpp
        telegraphtest.cpp
        utiltest.cpp
        )
target_link_libraries(solemnsky_tests
        gtest
        gtest_main
        solemnsky
        )
