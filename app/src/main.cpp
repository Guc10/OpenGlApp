// C++
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "MainWindow.h"
#include <iostream>
#include "Ball.h"
#include "Boundaries.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Helper: create a simple framebuffer + texture to render into
static GLuint CreateRenderTexture(int width, int height, GLuint &fboOut) {
    GLuint tex = 0, fbo = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer incomplete\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    fboOut = fbo;
    return tex;
}

static GLuint CompileShader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetShaderInfoLog(s, 1024, nullptr, buf);
        std::cerr << "Shader compile error: " << buf << "\n";
    }
    return s;
}

static GLuint CreateSimpleProgram() {
    const char *vs =
            "#version 330 core\n"
            "layout(location = 0) in vec2 aPos;\n"
            "void main(){ gl_Position = vec4(aPos, 0.0, 1.0); }\n";
    const char *fs =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec4 uColor;\n"
            "void main(){ FragColor = uColor; }\n";

    GLuint vsId = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint fsId = CompileShader(GL_FRAGMENT_SHADER, fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vsId);
    glAttachShader(prog, fsId);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, buf);
        std::cerr << "Program link error: " << buf << "\n";
    }
    glDeleteShader(vsId);
    glDeleteShader(fsId);
    return prog;
}

int main() {
    if (!glfwInit()) return -1;

    // Request OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow *window = glfwCreateWindow(1280, 720, "OpenGlApp", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Do NOT initialize ImGui here if MainWindow does it.
    // Construct UI wrapper (assumed to initialize ImGui)
    MainWindow ui(window);

    // Create scene FBO/texture and pass to UI
    const int sceneW = 800, sceneH = 600;
    GLuint sceneFbo = 0;
    GLuint sceneTex = CreateRenderTexture(sceneW, sceneH, sceneFbo);
    ui.setRenderTexture(sceneTex, sceneW, sceneH);

    // Create objects: ball + boundaries
    Ball ball(0.8f, 0.08f, 40); // reflectance, radius, segments
    Boundaries boundaries;

    // Shader to draw the ball (works in NDC space that Ball uses)
    GLuint ballProgram = CreateSimpleProgram();
    GLint colorLoc = glGetUniformLocation(ballProgram, "uColor");

    // Running control via UI callbacks
    bool running = true;
    ui.setOnStart([&]() { running = true; });
    ui.setOnStop([&]() { running = false; });
    ui.setOnGravityChanged([&](float g) { ball.SetGravity(g); });
    ui.setOnReflectanceChanged([&](float r) { ball.SetReflectance(r); });

    static bool mouseDown = false;
    static bool dragging = false;
    static glm::vec2 dragStartNDC(0.0f);
    static float launchPower = 1.0f;

    // Time tracking
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Poll events first so input queries reflect this frame
        glfwPollEvents();

        // time step
        double now = glfwGetTime();
        float dt = static_cast<float>(now - lastTime);
        lastTime = now;

        // Start ImGui frame early (after polling)
        ui.NewFrame();

        // --- Mouse / drag handling ---
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        int fbW = 1, fbH = 1;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        // Map cursor into the scene FBO coordinates.
        // This assumes the scene image in the UI is shown at size (sceneW, sceneH)
        // and is centered in the framebuffer. Adjust imgX/imgY if different.
        int imgX = (fbW - sceneW) / 2;
        int imgY = (fbH - sceneH) / 2;

        float localX = static_cast<float>(mx) - static_cast<float>(imgX);
        float localY = static_cast<float>(my) - static_cast<float>(imgY);
        bool insideScene =
                localX >= 0.0f && localY >= 0.0f && localX <= static_cast<float>(sceneW) && localY <= static_cast<float>
                (sceneH);

        glm::vec2 currNDC(0.0f, 0.0f);
        if (insideScene && sceneW > 0 && sceneH > 0) {
            currNDC.x = localX / static_cast<float>(sceneW) * 2.0f - 1.0f;
            currNDC.y = 1.0f - localY / static_cast<float>(sceneH) * 2.0f;
        }

        int mbState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (insideScene && mbState == GLFW_PRESS && !mouseDown) {
            mouseDown = true;
            dragging = true;
            dragStartNDC = currNDC;
            ball.SetPosition(currNDC);
        } else if (insideScene && mbState == GLFW_PRESS && mouseDown) {
            if (dragging) ball.SetPosition(currNDC);
        } else if (mbState == GLFW_RELEASE && mouseDown) {
            mouseDown = false;
            if (dragging) {
                glm::vec2 drag = currNDC - dragStartNDC;
                ball.LaunchFromDrag(drag, launchPower);
                dragging = false;
            }
        }

        // --- Physics update (only when running and not dragging) ---
        if (running && !dragging && dt > 0.0f) {
            ball.UpdatePosition(glm::vec2(0.0f), dt);
            boundaries.ResolveCollision(ball);
            ball.UpdateMesh();
        }

        // --- Render scene into FBO (unchanged) ---
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
        glViewport(0, 0, sceneW, sceneH);
        glClearColor(0.20f, 0.25f, 0.30f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (ball.bouncingBallVAO != 0) {
            glUseProgram(ballProgram);
            if (colorLoc >= 0)
                glUniform4f(colorLoc, 0.9f, 0.35f, 0.2f, 1.0f);
            glBindVertexArray(ball.bouncingBallVAO);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) ball.GetVertexCount());
            glDisable(GL_BLEND);
            glBindVertexArray(0);
            glUseProgram(0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Prepare screen and render UI (unchanged)
        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ui.Render(); // draws the UI + the scene texture inside the UI

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteProgram(ballProgram);
    ui.Shutdown(); // assume MainWindow handles ImGui shutdown

    glDeleteFramebuffers(1, &sceneFbo);
    glDeleteTextures(1, &sceneTex);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
