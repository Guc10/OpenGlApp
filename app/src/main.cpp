#include "config.h"

// Simple shader helpers
static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetShaderInfoLog(s, sizeof(buf), nullptr, buf);
        cerr << "Shader compile error: " << buf << endl;
    }
    return s;
}

static GLuint createProgram(const char* vsSrc, const char* fsSrc) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetProgramInfoLog(prog, sizeof(buf), nullptr, buf);
        cerr << "Program link error: " << buf << endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

int main() {
    if (!glfwInit()) {
        cerr << "Failed to init GLFW\n";
        return -1;
    }

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui + Triangle", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to init GLAD\n";
        return -1;
    }

    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Triangle setup: simple triangle in normalized device coords
    float verts[] = {
         0.0f,  0.6f, 0.0f,
        -0.6f, -0.6f, 0.0f,
         0.6f, -0.6f, 0.0f
    };
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    const char* vs = R"(
        #version 330 core
        layout(location = 0) in vec3 pos;
        uniform vec2 u_scale;
        void main() {
            vec2 scaled = pos.xy * u_scale;
            gl_Position = vec4(scaled, pos.z, 1.0);
        }
    )";
    const char* fs = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(0.95, 0.6, 0.2, 1.0);
        }
    )";
    GLuint program = createProgram(vs, fs);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) glfwSetWindowShouldClose(window, true);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // UI window containing a canvas area where the triangle will render
        ImGui::Begin("Triangle");
        ImVec2 contentAvail = ImGui::GetContentRegionAvail();
        // ensure a minimum size
        if (contentAvail.x < 10) contentAvail.x = 200;
        if (contentAvail.y < 10) contentAvail.y = 200;

        // Reserve the space in the layout
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("canvas", contentAvail);
        ImGui::End();

        // Render ImGui to get proper draw data and then render GL content to the reserved rectangle
        ImGui::Render();

        int fb_w, fb_h;
        glfwGetFramebufferSize(window, &fb_w, &fb_h);
        glViewport(0, 0, fb_w, fb_h);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render triangle confined to the ImGui rectangle using viewport + scissor
        int x = (int)canvasPos.x;
        int y = (int)canvasPos.y;
        int w = (int)contentAvail.x;
        int h = (int)contentAvail.y;
        // Convert ImGui screen Y (top-left origin) to GL framebuffer Y (bottom-left origin)
        int glY = fb_h - (y + h);

        if (w > 0 && h > 0) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(x, glY, w, h);
            glViewport(x, glY, w, h);

            glUseProgram(program);
            // set a scale so triangle fits nicely inside the widget
            float scaleX = 0.9f;
            float scaleY = 0.9f * (float)h / (float)w; // preserve aspect
            GLint loc = glGetUniformLocation(program, "u_scale");
            if (loc >= 0) glUniform2f(loc, scaleX, scaleY);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glUseProgram(0);

            glDisable(GL_SCISSOR_TEST);

            // restore full viewport
            glViewport(0, 0, fb_w, fb_h);
        }

        // Finally render ImGui on top
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}