cmake_minimum_required(VERSION 4.0)
project(OpenGlApp)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 REQUIRED)

add_executable(OpenGlApp
        src/main.cpp
        src/config.h
        dependencies/GLAD/glad.c
)

target_include_directories(OpenGlApp PRIVATE
        dependencies
        dependencies/GLAD
        dependencies/GLM
        dependencies/imgui
)

target_link_libraries(OpenGlApp glfw)