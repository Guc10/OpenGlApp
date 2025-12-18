#include "MainWindow.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <sstream>
#include "Ball.h"

MainWindow::MainWindow(GLFWwindow* window)
    : window_(window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

MainWindow::~MainWindow()
{
    Shutdown();
}

void MainWindow::Shutdown()
{
    if (!ImGui::GetCurrentContext()) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void MainWindow::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void MainWindow::setRenderTexture(uint32_t glTexId, int width, int height)
{
    renderTex_ = glTexId;
    if (width > 0) texWidth_ = width;
    if (height > 0) texHeight_ = height;
}

void MainWindow::Render()
{
    // Inicjalizacja shadera dla kulki
    static GLuint ballProgram = 0;
    if (ballProgram == 0) {
        const char* vsSrc =
            "#version 330 core\n"
            "layout(location = 0) in vec2 aPos;\n"
            "void main(){ gl_Position = vec4(aPos, 0.0, 1.0); }\n";
        const char* fsSrc =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec4 uColor;\n"
            "void main(){ FragColor = uColor; }\n";

        auto compile = [](GLenum type, const char* src)->GLuint {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            return s;
        };

        GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
        GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);
        ballProgram = glCreateProgram();
        glAttachShader(ballProgram, vs);
        glAttachShader(ballProgram, fs);
        glLinkProgram(ballProgram);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    // Ustawienie okna IMGUI
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Main Window", nullptr, window_flags);

    // Sprawdzenie błędów
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) { ImGui::Text("GL error: 0x%X", err); }

    // Wyświetlenie tekstury
    if (renderTex_ != 0) {
        ImVec2 imgSize = (texWidth_ > 0 && texHeight_ > 0)
            ? ImVec2((float)texWidth_, (float)texHeight_)
            : ImVec2(640, 480);

        const float maxW = ImGui::GetContentRegionAvail().x;
        if (imgSize.x > maxW) {
            float scale = maxW / imgSize.x;
            imgSize.x *= scale;
            imgSize.y *= scale;
        }

        ImVec2 avail = ImGui::GetContentRegionAvail();

        float offsetX = (avail.x - imgSize.x) * 0.5f;
        if (offsetX < 0) offsetX = 0;

        float offsetY = (avail.y - imgSize.y) * 0.5f;
        if (offsetY < 0) offsetY = 0;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

        ImGui::Image((void*)(intptr_t)renderTex_, imgSize, ImVec2(0,1), ImVec2(1,0));
    } else {
        ImGui::TextWrapped("No render texture set. Set with setRenderTexture(glTexId, width, height).");
    }

    ImGui::Separator();

    // Sterowanie symulacją
    if (!running_) {
        if (ImGui::Button("Start")) { running_ = true; if (onStart_) onStart_(); }
    } else {
        if (ImGui::Button("Stop")) { running_ = false; if (onStop_) onStop_(); }
    }
    ImGui::SameLine();
    ImGui::Text("Running: %s", running_ ? "Yes" : "No");

    float prevG = gravity_;
    ImGui::PushItemWidth(150.0f);
    ImGui::InputFloat("Gravity (min: 0, max: 20.0)", &gravity_, 1.0f, 1.0f, "%.2f");
    ImGui::PopItemWidth();
    gravity_ = std::clamp(gravity_, 0.0f, 20.0f);

    float prevR = reflectance_;
    ImGui::PushItemWidth(150.0f);
    ImGui::InputFloat("Reflectance (min: 0.1, max: 1.0)", &reflectance_, 0.1f, 0.1f, "%.1f");
    ImGui::PopItemWidth();
    reflectance_ = std::clamp(reflectance_, 0.1f, 1.0f);

    float prevRadius = radius_;
    ImGui::PushItemWidth(150.0f);
    ImGui::InputFloat("Ball radius (min: 0.06, max: 0.5)", &radius_, 0.01f, 0.1f, "%.2f");
    ImGui::PopItemWidth();
    radius_ = std::clamp(radius_, 0.06f, 0.5f);

    ImGui::PushItemWidth(50.0f);

    if (ImGui::Checkbox("Dark Mode", &themeDark_))
    {
        if (themeDark_) ImGui::StyleColorsDark();
        else ImGui::StyleColorsLight();
    }


    if (gravity_ != prevG) {
        if (onGravityChanged_) onGravityChanged_(gravity_);
    }
    if (reflectance_ != prevR) {

        if (onReflectanceChanged_) onReflectanceChanged_(reflectance_);
    }
    if (radius_ != prevRadius) {
        if (onRadiusChanged_) onRadiusChanged_(radius_);
    }

    // Renderowanie okna
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
