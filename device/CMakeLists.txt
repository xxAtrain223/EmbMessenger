cmake_minimum_required(VERSION 3.1)
project(EmbMessengerDevice LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_link_libraries(${PROJECT_NAME} INTERFACE EmbMessengerShared)

file(GLOB_RECURSE ${PROJECT_NAME}_TEST_SOURCES "test/*.[ch]pp")
add_executable(${PROJECT_NAME}Test ${${PROJECT_NAME}_TEST_SOURCES})
set_target_properties(${PROJECT_NAME}Test PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED YES)
target_link_libraries(${PROJECT_NAME}Test ${PROJECT_NAME} gtest gmock gtest_main)
target_compile_options(${PROJECT_NAME}Test PRIVATE -g -O0 --coverage -DEMB_TESTING)
set_target_properties(${PROJECT_NAME}Test PROPERTIES LINK_FLAGS "--coverage")
