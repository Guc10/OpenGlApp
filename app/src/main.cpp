#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "objects/FluidSim.h"
#include "objects/MainWindow.h"

// ---------------------------------------------------------------------------
// FBO helpers
// ---------------------------------------------------------------------------

static GLuint CreateRenderTexture(int width, int height, GLuint& fboOut) {
    GLuint tex = 0, fbo = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer incomplete\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    fboOut = fbo;
    return tex;
}

// ---------------------------------------------------------------------------
// Shader helpers
// ---------------------------------------------------------------------------

static GLuint CompileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetShaderInfoLog(s, 1024, nullptr, buf);
        std::cerr << "Shader error: " << buf << "\n";
    }
    return s;
}

static GLuint LinkProgram(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, buf);
        std::cerr << "Link error: " << buf << "\n";
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// Particle program: instanced circles with speed-based colour blending
static GLuint CreateParticleProgram() {
    const char* vs =
        "#version 330 core\n"
        "layout(location=0) in vec2 aVertex;\n"       // unit-circle point
        "layout(location=1) in vec3 aInstance;\n"     // xy=pos, z=speed01
        "uniform float uRadius;\n"
        "out float vSpeed;\n"
        "void main(){\n"
        "  vSpeed = aInstance.z;\n"
        "  gl_Position = vec4(aVertex * uRadius + aInstance.xy, 0.0, 1.0);\n"
        "}\n";

    const char* fs =
        "#version 330 core\n"
        "in  float vSpeed;\n"
        "out vec4  FragColor;\n"
        "uniform vec3 uColorLow;\n"
        "uniform vec3 uColorHigh;\n"
        "void main(){\n"
        "  vec3 col = mix(uColorLow, uColorHigh, vSpeed);\n"
        "  // soft circular edge\n"
        "  FragColor = vec4(col, 0.88);\n"
        "}\n";

    return LinkProgram(
        CompileShader(GL_VERTEX_SHADER,   vs),
        CompileShader(GL_FRAGMENT_SHADER, fs));
}

// Simple flat-colour program for scene geometry
static GLuint CreateSceneProgram() {
    const char* vs =
        "#version 330 core\n"
        "layout(location=0) in vec2 aPos;\n"
        "void main(){ gl_Position = vec4(aPos, 0.0, 1.0); }\n";

    const char* fs =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 uColor;\n"
        "void main(){ FragColor = uColor; }\n";

    return LinkProgram(
        CompileShader(GL_VERTEX_SHADER,   vs),
        CompileShader(GL_FRAGMENT_SHADER, fs));
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Simulation", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ---- Scene FBO ----
    const int sceneW = 800, sceneH = 600;
    GLuint sceneFbo = 0;
    GLuint sceneTex = CreateRenderTexture(sceneW, sceneH, sceneFbo);

    // ---- Shaders ----
    GLuint particleProg = CreateParticleProgram();
    GLuint sceneProg    = CreateSceneProgram();

    GLint uRadius    = glGetUniformLocation(particleProg, "uRadius");
    GLint uColorLow  = glGetUniformLocation(particleProg, "uColorLow");
    GLint uColorHigh = glGetUniformLocation(particleProg, "uColorHigh");
    GLint uColor     = glGetUniformLocation(sceneProg,    "uColor");

    // ---- Simulation ----
    FluidSim fluid;

    // ---- UI ----
    MainWindow ui(window);
    ui.setRenderTexture(sceneTex, sceneW, sceneH);

    ui.setOnStart(           [&]()      { fluid.SetRunning(true);          });
    ui.setOnStop(            [&]()      { fluid.SetRunning(false);         });
    ui.setOnReset(           [&]()      { fluid.Reset();                   });
    ui.setOnGravityChanged(  [&](float g){ fluid.SetGravity(g);           });
    ui.setOnViscosityChanged([&](float v){ fluid.SetViscosity(v);         });
    ui.setOnQualityChanged(  [&](int   q){ fluid.SetQuality(q);           });
    ui.setOnRenderRadiusChanged([&](float r){ fluid.SetRenderRadius(r);   });
    ui.setOnColorChanged([&](float r, float g, float b){
        fluid.SetBaseColor({r, g, b});
    });

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double now = glfwGetTime();
        float  dt  = static_cast<float>(now - lastTime);
        lastTime   = now;

        ui.NewFrame();

        // ---- Simulate ----
        fluid.Update(dt);

        // ---- Render scene to FBO ----
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
        glViewport(0, 0, sceneW, sceneH);
        glClearColor(0.13f, 0.15f, 0.19f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        fluid.RenderScene(sceneProg, uColor);
        fluid.RenderParticles(particleProg, uRadius, uColorLow, uColorHigh);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ---- Render window ----
        int dw, dh;
        glfwGetFramebufferSize(window, &dw, &dh);
        glViewport(0, 0, dw, dh);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ui.Render(fluid.GetParticleCount());

        glfwSwapBuffers(window);
    }

    // ---- Cleanup ----
    glDeleteProgram(particleProg);
    glDeleteProgram(sceneProg);
    glDeleteFramebuffers(1, &sceneFbo);
    glDeleteTextures(1, &sceneTex);
    ui.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
