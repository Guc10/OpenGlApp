// main.cpp
#include "config.h"
#include "objects/Boundaries.h"
#include "objects/Ball.h"

// Function for loading shaders from files
std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(std::string(SHADER_DIR) + filePath);
    if (!file.is_open()) {
        std::filesystem::path current = std::filesystem::current_path();
        std::cerr << "Can't open given file: " << filePath << "| Current directory: " << current.string() <<  std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Loading shaders from files
std::string vertexShaderSourceStr = loadShaderSource("main.vert");
const char* vertexShaderSource = vertexShaderSourceStr.c_str();

std::string fragmentShaderSourceRedStr = loadShaderSource("red.frag");
const char* fragmentShaderSourceRed = fragmentShaderSourceRedStr.c_str();

std::string fragmentShaderSourceGreenStr = loadShaderSource("green.frag");
const char* fragmentShaderSourceGreen = fragmentShaderSourceGreenStr.c_str();

std::string fragmentShaderSourceBlueStr = loadShaderSource("blue.frag");
const char* fragmentShaderSourceBlue = fragmentShaderSourceBlueStr.c_str();

// Input processing
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize main window
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Big Bouncing Ball", NULL, NULL);
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

    // Generate bouncing ball
    const float radius = 0.1f;
    const unsigned int num_segments = 20;
    auto *ball = new Ball(0.8f, radius, num_segments);

    // Generate boundaries
    auto *boundaries = new Boundaries();

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

    // Blue fragment shader
    GLuint fragmentShaderBlue = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderBlue, 1, &fragmentShaderSourceBlue, nullptr);
    glCompileShader(fragmentShaderBlue);

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

    // Link blue shader program
    GLuint shaderProgramBlue = glCreateProgram();
    glAttachShader(shaderProgramBlue, vertexShader);
    glAttachShader(shaderProgramBlue, fragmentShaderBlue);
    glLinkProgram(shaderProgramBlue);

    // Clean up shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderRed);
    glDeleteShader(fragmentShaderGreen);
    glDeleteShader(fragmentShaderBlue);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Time necessary for calculating ball movement
        double currentTime = glfwGetTime();
        float delta = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        // Input
        processInput(window);

        // Ball movement
        ball->UpdatePosition({}, delta);

        // Handle collisions
        boundaries->ResolveCollision(*ball);

        // Clear last frame
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ball->UpdateMesh();

        // Draw ball (red)
        glUseProgram(shaderProgramRed);
        glBindVertexArray(ball->bouncingBallVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments + 2);

        // Draw boundaries (blue)
        glUseProgram(shaderProgramBlue);
        glBindVertexArray(boundaries->boundariesVAO);
        glDrawElements(GL_TRIANGLES, 27, GL_UNSIGNED_INT, 0);

        // Swap and poll
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete ball and boundaries objects
    delete ball;
    delete boundaries;

    glfwTerminate();
    return 0;
}