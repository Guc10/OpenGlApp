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

    if (ball_ && ball_->bouncingBallVAO != 0) {
        ball_->UpdateMesh();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(ballProgram);
        GLint loc = glGetUniformLocation(ballProgram, "uColor");
        if (loc >= 0) glUniform4f(loc, 0.9f, 0.3f, 0.2f, 1.0f);
        glBindVertexArray(ball_->bouncingBallVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)ball_->GetVertexCount());
        glBindVertexArray(0);
        glUseProgram(0);
        glDisable(GL_BLEND);
    }
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

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) { ImGui::Text("GL error: 0x%X", err); }

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

        //ImGui::Text("Thumbnail:");
        //ImGui::Image((void*)(intptr_t)renderTex_, ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0));

    } else {
        ImGui::TextWrapped("No render texture set. Set with setRenderTexture(glTexId, width, height).");
    }

    ImGui::Separator();

    if (!running_) {
        if (ImGui::Button("Start")) { running_ = true; if (onStart_) onStart_(); }
    } else {
        if (ImGui::Button("Stop")) { running_ = false; if (onStop_) onStop_(); }
    }
    ImGui::SameLine();
    ImGui::Text("Running: %s", running_ ? "Yes" : "No");

    float prevG = gravity_;
    ImGui::PushItemWidth(150.0f);
    ImGui::InputFloat("Gravity", &gravity_, 1.0f, 1.0f, "%.2f");
    ImGui::PopItemWidth();

    float prevR = reflectance_;
    ImGui::PushItemWidth(150.0f);
    ImGui::InputFloat("Reflectance (min: 0, max: 1.0)", &reflectance_, 0.1f, 0.1f, "%.1f");
    ImGui::PopItemWidth();
    reflectance_ = std::clamp(reflectance_, 0.1f, 1.0f);

    if (gravity_ != prevG) {
        if (onGravityChanged_) onGravityChanged_(gravity_);
    }
    if (reflectance_ != prevR) {

        if (onReflectanceChanged_) onReflectanceChanged_(reflectance_);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
