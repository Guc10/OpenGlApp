#pragma once
#include <glad/glad.h>
#include <functional>
#include <cstdint>
#include <GLFW/glfw3.h>

class MainWindow {
public:
    explicit MainWindow(GLFWwindow* window);
    ~MainWindow();

    void NewFrame();
    void Render(int particleCount);
    void Shutdown();

    void setRenderTexture(uint32_t glTexId, int width = 0, int height = 0);

    // Fluid property callbacks
    void setOnStart(std::function<void()> cb)                    { onStart_               = std::move(cb); }
    void setOnStop(std::function<void()> cb)                     { onStop_                = std::move(cb); }
    void setOnReset(std::function<void()> cb)                    { onReset_               = std::move(cb); }
    void setOnGravityChanged(std::function<void(float)> cb)      { onGravityChanged_      = std::move(cb); }
    void setOnViscosityChanged(std::function<void(float)> cb)    { onViscosityChanged_    = std::move(cb); }
    void setOnQualityChanged(std::function<void(int)> cb)        { onQualityChanged_      = std::move(cb); }
    void setOnRenderRadiusChanged(std::function<void(float)> cb) { onRenderRadiusChanged_ = std::move(cb); }
    void setOnColorChanged(std::function<void(float,float,float)> cb) { onColorChanged_   = std::move(cb); }

    bool isRunning() const { return running_; }

private:
    GLFWwindow* window_;
    uint32_t renderTex_ = 0;
    int texWidth_ = 0, texHeight_ = 0;

    bool  running_      = true;
    float gravity_      = 2.5f;
    float viscosity_    = 1.2f;
    int   quality_      = 3;
    float renderRadius_ = 0.022f;
    bool  themeDark_    = true;
    float color_[3]     = {0.15f, 0.55f, 1.0f};

    std::function<void()>           onStart_;
    std::function<void()>           onStop_;
    std::function<void()>           onReset_;
    std::function<void(float)>      onGravityChanged_;
    std::function<void(float)>      onViscosityChanged_;
    std::function<void(int)>        onQualityChanged_;
    std::function<void(float)>      onRenderRadiusChanged_;
    std::function<void(float,float,float)> onColorChanged_;
};
