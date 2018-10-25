cmake_minimum_required(VERSION 3.0.0)
project(serial)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES "src/*")

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src)
