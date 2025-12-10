cmake_minimum_required(VERSION 4.0)
project(OpenGlApp)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
        main.cpp
        objects/Ball.cpp
        objects/Boundaries.cpp
        objects/MainWindow.cpp
)

set(HEADERS
        config.h
        objects/Ball.h
        objects/Boundaries.h
        objects/MainWindow.h
)

set(IMGUI
        ../dependencies/ImGui/imgui.cpp
        ../dependencies/ImGui/imgui_draw.cpp
        ../dependencies/ImGui/imgui_widgets.cpp
        ../dependencies/ImGui/imgui_tables.cpp
        ../dependencies/ImGui/imgui_demo.cpp
        ../dependencies/ImGui/backends/imgui_impl_glfw.cpp
        ../dependencies/ImGui/backends/imgui_impl_opengl3.cpp
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
        ../dependencies/ImGui
        ../dependencies/ImGui/backends
)

find_package(glfw3 REQUIRED)
target_link_libraries(OpenGlApp PRIVATE glfw)

add_definitions(-DSHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\")
