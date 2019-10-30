cmake_minimum_required(VERSION 3.0.0)
project(EmbMessenger)

option(${PROJECT_NAME}_ENABLE_TESTING "Enable Testing for ${PROJECT_NAME}" OFF)
option(BUILD_EXAMPLES "Build the examples" OFF)

add_subdirectory(shared)
add_subdirectory(host)

if (${PROJECT_NAME}_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(ext)
    add_subdirectory(device)

    cmake_policy(SET CMP0057 NEW)
    include(GoogleTest)
    gtest_add_tests(EmbMessengerSharedTest "" AUTO)
    gtest_add_tests(EmbMessengerHostTest "" AUTO)
    gtest_add_tests(EmbMessengerDeviceTest "" AUTO)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
