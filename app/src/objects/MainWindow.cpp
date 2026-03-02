#include "MainWindow.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <algorithm>

MainWindow::MainWindow(GLFWwindow* window) : window_(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

MainWindow::~MainWindow() { Shutdown(); }

void MainWindow::Shutdown() {
    if (!ImGui::GetCurrentContext()) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void MainWindow::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void MainWindow::setRenderTexture(uint32_t glTexId, int width, int height) {
    renderTex_ = glTexId;
    if (width  > 0) texWidth_  = width;
    if (height > 0) texHeight_ = height;
}

void MainWindow::Render(int particleCount) {
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Fluid Simulation", nullptr, flags);

    // ---- Scene texture ----
    if (renderTex_ != 0) {
        ImVec2 imgSize = (texWidth_ > 0 && texHeight_ > 0)
            ? ImVec2((float)texWidth_, (float)texHeight_)
            : ImVec2(640, 480);

        float maxW = ImGui::GetContentRegionAvail().x;
        if (imgSize.x > maxW) {
            float scale = maxW / imgSize.x;
            imgSize.x *= scale;
            imgSize.y *= scale;
        }
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float offsetX = std::max(0.0f, (avail.x - imgSize.x) * 0.5f);
        float offsetY = std::max(0.0f, (avail.y - imgSize.y) * 0.5f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
        ImGui::Image((void*)(intptr_t)renderTex_, imgSize, ImVec2(0,1), ImVec2(1,0));
    }

    ImGui::Separator();

    // ---- Playback controls ----
    ImGui::Text("Particles: %d", particleCount);
    ImGui::SameLine(160);
    if (!running_) {
        if (ImGui::Button("  Start  ")) { running_ = true;  if (onStart_) onStart_(); }
    } else {
        if (ImGui::Button("  Stop   ")) { running_ = false; if (onStop_)  onStop_();  }
    }
    ImGui::SameLine();
    if (ImGui::Button("  Reset  ")) { if (onReset_) onReset_(); }
    ImGui::SameLine();
    ImGui::TextDisabled(running_ ? "[running]" : "[paused]");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Fluid Properties");
    ImGui::Separator();
    ImGui::Spacing();

    // ---- Gravity ----
    float prevG = gravity_;
    ImGui::PushItemWidth(160.f);
    ImGui::SliderFloat("Gravity", &gravity_, 0.0f, 10.0f, "%.2f");
    ImGui::PopItemWidth();
    if (gravity_ != prevG && onGravityChanged_) onGravityChanged_(gravity_);

    // ---- Viscosity ----
    float prevV = viscosity_;
    ImGui::PushItemWidth(160.f);
    ImGui::SliderFloat("Viscosity", &viscosity_, 0.0f, 5.0f, "%.2f");
    ImGui::PopItemWidth();
    if (viscosity_ != prevV && onViscosityChanged_) onViscosityChanged_(viscosity_);

    // ---- Quality (spawn rate) ----
    int prevQ = quality_;
    ImGui::PushItemWidth(160.f);
    ImGui::SliderInt("Quality (flow rate)", &quality_, 1, 10);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::TextDisabled("particles/burst");
    if (quality_ != prevQ && onQualityChanged_) onQualityChanged_(quality_);

    // ---- Particle render radius ----
    float prevRad = renderRadius_;
    ImGui::PushItemWidth(160.f);
    ImGui::SliderFloat("Particle size", &renderRadius_, 0.022f, 0.05f, "%.3f");
    ImGui::PopItemWidth();
    if (renderRadius_ != prevRad && onRenderRadiusChanged_) onRenderRadiusChanged_(renderRadius_);

    // ---- Color ----
    float prevCol[3] = {color_[0], color_[1], color_[2]};
    ImGui::ColorEdit3("Fluid color", color_);
    if ((color_[0] != prevCol[0] || color_[1] != prevCol[1] || color_[2] != prevCol[2])
        && onColorChanged_)
    {
        onColorChanged_(color_[0], color_[1], color_[2]);
    }

    ImGui::Spacing();

    // ---- Theme ----
    if (ImGui::Checkbox("Dark mode", &themeDark_)) {
        if (themeDark_) ImGui::StyleColorsDark();
        else            ImGui::StyleColorsLight();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextDisabled("Teal rectangle = fluid source");
    ImGui::TextDisabled("Gray triangle  = obstacle / ramp");

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
