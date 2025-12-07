#pragma once
#include<glad/glad.h>
#include <functional>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "Ball.h"

class MainWindow {
public:
    explicit MainWindow(GLFWwindow* window);
    ~MainWindow();

    // Call at start of each frame
    void NewFrame();

    // Build and render the ImGui UI for this window
    void Render();

    // Cleanup ImGui
    void Shutdown();

    // Set the OpenGL texture id that contains the rendered scene
    void setRenderTexture(uint32_t glTexId, int width = 0, int height = 0);

    // Callbacks
    void setOnStart(std::function<void()> cb) { onStart_ = std::move(cb); }
    void setOnStop(std::function<void()> cb) { onStop_ = std::move(cb); }
    void setOnGravityChanged(std::function<void(float)> cb) { onGravityChanged_ = std::move(cb); }
    void setOnReflectanceChanged(std::function<void(float)> cb) { onReflectanceChanged_ = std::move(cb); }

    bool isRunning() const { return running_; }
    float gravity() const { return gravity_; }

private:
    GLFWwindow* window_;
    uint32_t renderTex_ = 0;
    int texWidth_ = 0, texHeight_ = 0;
    Ball* ball_ = nullptr;

    bool running_ = false;
    float gravity_ = 9.81f;
    float reflectance_ = 0.8f;

    std::function<void()> onStart_;
    std::function<void()> onStop_;
    std::function<void(float)> onGravityChanged_;
    std::function<void(float)> onReflectanceChanged_;
};
