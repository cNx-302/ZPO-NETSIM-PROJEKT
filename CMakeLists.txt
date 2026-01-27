cmake_minimum_required(VERSION 3.15)
project(ZPO_NETSIM_PROJEKT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(NETSIM_SOURCES
        src/package.cpp
        src/storage_types.cpp
        src/nodes.cpp
        src/helpers.cpp
	src/factory.cpp
)


add_executable(NetSim_App main.cpp ${NETSIM_SOURCES})


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(NetSim_Tests
        #tests/tests_1_package.cpp
        tests/tests_2_nodes.cpp
	tests/tests_3_factory.cpp
        ${NETSIM_SOURCES}
)

target_link_libraries(NetSim_Tests GTest::gtest_main)