cmake_minimum_required(VERSION 4.0)
project(OpenGlApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- GLFW ----------
add_subdirectory(dependencies/glfw)

# ---------- GLM ----------
add_subdirectory(dependencies/glm)

# ---------- GLAD ----------
add_library(glad STATIC
        dependencies/glad/src/glad.c
)

target_include_directories(glad PUBLIC
        dependencies/glad/include
)

# ---------- ImGui ----------
add_library(imgui STATIC
        dependencies/imgui/imgui.cpp
        dependencies/imgui/imgui_draw.cpp
        dependencies/imgui/imgui_tables.cpp
        dependencies/imgui/imgui_widgets.cpp
        dependencies/imgui/backends/imgui_impl_glfw.cpp
        dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        dependencies/imgui
        dependencies/imgui/backends
)

target_link_libraries(imgui PUBLIC glfw glad)

# ---------- App ----------
add_executable(OpenGlApp
        src/main.cpp
        src/objects/Ball.cpp
        src/objects/Boundaries.cpp
        src/objects/MainWindow.cpp
)

target_link_libraries(OpenGlApp
        glfw
        glad
        imgui
        glm::glm
)