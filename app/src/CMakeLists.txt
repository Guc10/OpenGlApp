cmake_minimum_required(VERSION 4.0)
project(OpenGlApp)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
    main.cpp
    physics/Ball.cpp
    physics/Movement.cpp
    physics/Boundaries.cpp
    renders/LeftWall.cpp
    renders/RightWall.cpp
    renders/Floor.cpp
    renders/BouncingBall.cpp
)

set(HEADERS
    config.h
    physics/Ball.h
    physics/Movement.h
    physics/Boundaries.h
    renders/LeftWall.h
    renders/RightWall.h
    renders/Floor.h
    renders/BouncingBall.h
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
        renders/Floor.cpp
)

add_executable(OpenGlApp
    ${SOURCES}
    ${HEADERS}
    ${IMGUI}
    ${GLAD}
)

target_include_directories(OpenGlApp PRIVATE
    ../dependencies
    ../dependencies/GLAD
    ../dependencies/GLM
    ../dependencies/imgui
)

find_package(glfw3 REQUIRED)
target_link_libraries(OpenGlApp PRIVATE glfw)

add_definitions(-DSHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\")
