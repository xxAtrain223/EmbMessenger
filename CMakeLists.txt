cmake_minimum_required(VERSION 3.0.0)
project(EmbMessenger)

option(EMB_ENABLE_TESTING "Enable Testing for EmbMessenger" OFF)
option(BUILD_EXAMPLES "Build the examples" OFF)

add_subdirectory(shared)
add_subdirectory(host)

if (EMB_ENABLE_TESTING)
    set(gtest_force_shared_crt TRUE CACHE BOOL "")
    add_subdirectory(ext)
    add_subdirectory(device)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
