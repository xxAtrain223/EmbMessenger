cmake_minimum_required(VERSION 3.0.0)

set(gtest_force_shared_crt TRUE CACHE BOOL "")
add_subdirectory(ext)
add_subdirectory(shared)
add_subdirectory(host)
