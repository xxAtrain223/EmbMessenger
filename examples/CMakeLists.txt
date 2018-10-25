cmake_minimum_required(VERSION 3.0.0)
project(EmbMessengerExamples)

if (BUILD_EXAMPLES)
    file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS "host/include/*.hpp")
    file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES "host/src/*.cpp")

    add_subdirectory("host/ext/serial")

    add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED YES)
    target_include_directories(${PROJECT_NAME} PUBLIC "host/include")
    target_link_libraries(${PROJECT_NAME} EmbMessengerHost serial)
endif()