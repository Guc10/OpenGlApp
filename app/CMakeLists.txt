cmake_minimum_required(VERSION 4.0)
project(test5)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(test5
        src/main.cpp
        src/config.h
)

target_include_directories(test5 PRIVATE dependencies)

target_link_libraries(test5 glfw OpenGL::GL)