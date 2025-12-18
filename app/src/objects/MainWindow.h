#pragma once
#include<glad/glad.h>
#include <functional>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "Ball.h"
#include "Boundaries.h"

class MainWindow {
public:
    explicit MainWindow(GLFWwindow* window);
    ~MainWindow();

    void NewFrame();

    void Render();

    void Shutdown();

    void setRenderTexture(uint32_t glTexId, int width = 0, int height = 0);

    void setOnStart(std::function<void()> cb) { onStart_ = std::move(cb); }
    void setOnStop(std::function<void()> cb) { onStop_ = std::move(cb); }
    void setOnGravityChanged(std::function<void(float)> cb) { onGravityChanged_ = std::move(cb); }
    void setOnReflectanceChanged(std::function<void(float)> cb) { onReflectanceChanged_ = std::move(cb); }
    void setOnRadiusChanged(std::function<void(float)> cb) { onRadiusChanged_ = std::move(cb); }

    bool isRunning() const { return running_; }
    float gravity() const { return gravity_; }

private:
    GLFWwindow* window_;
    uint32_t renderTex_ = 0;
    int texWidth_ = 0, texHeight_ = 0;
    // Ball* ball_ = nullptr;
    // Boundaries* boundaries_ = nullptr;


    bool running_ = true;
    float gravity_ = 9.81f;
    float reflectance_ = 0.8f;
    float radius_ = 0.08f;
    bool themeDark_ = true;

    std::function<void()> onStart_;
    std::function<void()> onStop_;
    std::function<void(float)> onGravityChanged_;
    std::function<void(float)> onReflectanceChanged_;
    std::function<void(float)> onRadiusChanged_;
};
