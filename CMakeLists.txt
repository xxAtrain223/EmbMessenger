cmake_minimum_required(VERSION 3.0.0)
project(EmbMessenger)

set(gtest_force_shared_crt TRUE CACHE BOOL "")
add_subdirectory(ext)

option(ENABLE_TESTING "Enable Testing" ON)

add_subdirectory(shared)
add_subdirectory(host)
add_subdirectory(device)
