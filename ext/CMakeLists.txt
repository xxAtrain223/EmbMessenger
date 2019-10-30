cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ext)

if (NOT TARGET gtest AND EmbMessenger_ENABLE_TESTING)
    set(gtest_force_shared_crt TRUE CACHE BOOL "")
    add_subdirectory(googletest)
endif()
