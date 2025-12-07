cmake_minimum_required(VERSION 4.0)
project(OpenGlApp)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
        main.cpp
        objects/Ball.cpp
        objects/Movement.cpp
        objects/Boundaries.cpp
        objects/MainWindow.cpp
)

set(HEADERS
        config.h
        objects/Ball.h
        objects/Movement.h
        objects/Boundaries.h
        objects/MainWindow.h
)

set(IMGUI
        ../dependencies/imgui/imgui.cpp
        ../dependencies/imgui/imgui_draw.cpp
        ../dependencies/imgui/imgui_widgets.cpp
        ../dependencies/imgui/imgui_tables.cpp
        ../dependencies/imgui/imgui_demo.cpp
        ../dependencies/imgui/imgui_impl_glfw.cpp
        ../dependencies/imgui/imgui_impl_opengl3.cpp
)

set(GLAD
        ../dependencies/GLAD/glad.c
)

add_executable(OpenGlApp
        ${SOURCES}
        ${HEADERS}
        ${IMGUI}
        ${GLAD}
)

target_include_directories(OpenGlApp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/objects
        ../dependencies
        ../dependencies/GLAD
        ../dependencies/GLM
        ../dependencies/imgui
)

find_package(glfw3 REQUIRED)
target_link_libraries(OpenGlApp PRIVATE glfw)

add_definitions(-DSHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\")
