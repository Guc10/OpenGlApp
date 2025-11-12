// main.cpp
#include "config.h"

std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file("../src/shaders/" + filePath);
    if (!file.is_open()) {
        std::filesystem::path cureent = std::filesystem::current_path();
        std::cerr << "Can't open given file: " << filePath << "| Current directory: " << cureent.string() <<  std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    return content;
}

std::string vertexShaderSourceStr = loadShaderSource("main.vert");
const char* vertexShaderSource = vertexShaderSourceStr.c_str();

std::string fragmentShaderSourceRedStr = loadShaderSource("red.frag");
const char* fragmentShaderSourceRed = fragmentShaderSourceRedStr.c_str();

std::string fragmentShaderSourceGreenStr = loadShaderSource("green.frag");
const char* fragmentShaderSourceGreen = fragmentShaderSourceGreenStr.c_str();


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(480, 480, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Generate circle vertices
    const int num_segments = 20l;
    float vertices[2 * (num_segments + 2)];
    float cx = 0.0f, cy = 0.0f, r = 0.4f;
    vertices[0] = cx;
    vertices[1] = cy;
    for (int i = 0; i <= num_segments; ++i) {
        float theta = 2.0f * M_PIf * float(i) / float(num_segments);
        vertices[2 * (i + 1)] = cx + r * cosf(theta);
        vertices[2 * (i + 1) + 1] = cy + r * sinf(theta);
    }

    // VAO & VBO
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Triangle vertices

    float triVertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Triangle VAO & VBO
    GLuint triVAO, triVBO;

    glGenVertexArrays(1, &triVAO);
    glBindVertexArray(triVAO);

    glGenBuffers(1, &triVBO);
    glBindBuffer(GL_ARRAY_BUFFER,triVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);



    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Red fragment shader
    GLuint fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderRed, 1, &fragmentShaderSourceRed, nullptr);
    glCompileShader(fragmentShaderRed);

    // Green fragment shader
    GLuint fragmentShaderGreen = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderGreen, 1, &fragmentShaderSourceGreen, nullptr);
    glCompileShader(fragmentShaderGreen);

    // Link red shader program
    GLuint shaderProgramRed = glCreateProgram();
    glAttachShader(shaderProgramRed, vertexShader);
    glAttachShader(shaderProgramRed, fragmentShaderRed);
    glLinkProgram(shaderProgramRed);

    // Link green shader program
    GLuint shaderProgramGreen = glCreateProgram();
    glAttachShader(shaderProgramGreen, vertexShader);
    glAttachShader(shaderProgramGreen, fragmentShaderGreen);
    glLinkProgram(shaderProgramGreen);

    // Clean up shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderRed);
    glDeleteShader(fragmentShaderGreen);

    // Set polygon mode to line (wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set red color
        glUseProgram(shaderProgramRed);

        // Draw circle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments + 2);

        // Set green color
        glUseProgram(shaderProgramGreen);

        // Draw triangle
        glBindVertexArray(triVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        processInput(window);

        ImGui::Begin("Settings");
        ImGui::Text("1. _");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
