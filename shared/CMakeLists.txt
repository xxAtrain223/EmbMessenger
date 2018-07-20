cmake_minimum_required(VERSION 3.0.0)
project(EmbeddedMessengerShared LANGUAGES CXX)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS "include/*.hpp")
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
target_include_directories(${PROJECT_NAME} PUBLIC include)


file(GLOB_RECURSE ${PROJECT_NAME}_TEST_SOURCES "test/*.[ch]pp")

add_executable(${PROJECT_NAME}Test ${${PROJECT_NAME}_TEST_SOURCES})
set_property(TARGET ${PROJECT_NAME}Test PROPERTY CXX_STANDARD 14)
target_link_libraries(${PROJECT_NAME}Test ${PROJECT_NAME} gtest gmock gtest_main)
