// main.cpp
#include "config.h"

std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(std::string(SHADER_DIR) + filePath);
    if (!file.is_open()) {
        std::filesystem::path current = std::filesystem::current_path();
        std::cerr << "Can't open given file: " << filePath << "| Current directory: " << current.string() <<  std::endl;
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

std::string fragmentShaderSourceBlueStr = loadShaderSource("blue.frag");
const char* fragmentShaderSourceBlue = fragmentShaderSourceBlueStr.c_str();


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    // Left wall vertices & indices
    float leftWallVertices[] = {
        -0.9f,  1.0f, 0.0f,  // top right
        -0.9f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left
    };

    unsigned int leftWallIndices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    // Left wall VAO & VBO & EBO
    GLuint leftWallVAO, leftWallVBO, leftWallEBO;

    glGenVertexArrays(1, &leftWallVAO);
    glBindVertexArray(leftWallVAO);

    glGenBuffers(1, &leftWallVBO);
    glBindBuffer(GL_ARRAY_BUFFER, leftWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftWallVertices), leftWallVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &leftWallEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftWallEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(leftWallIndices), leftWallIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Right wall vertices & indices
    float rightWallVertices[] = {
        0.9f,  1.0f, 0.0f,  // top right
        0.9f, -1.0f, 0.0f,  // bottom right
        1.0f, -1.0f, 0.0f,  // bottom left
        1.0f,  1.0f, 0.0f   // top left
    };

    unsigned int rightWallIndices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    // Right wall VAO & VBO & EBO
    GLuint rightWallVAO, rightWallVBO, rightWallEBO;

    glGenVertexArrays(1, &rightWallVAO);
    glBindVertexArray(rightWallVAO);

    glGenBuffers(1, &rightWallVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rightWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightWallVertices), rightWallVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &rightWallEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightWallEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rightWallIndices), rightWallIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Floor vertices & indices
    float floorVertices[] = {
        1.0f,  -0.9f, 0.0f,  // top right
        1.0f,  -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f, -0.9f, 0.0f   // top left
    };

    unsigned int floorIndices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    // Right wall VAO & VBO & EBO
    GLuint floorVAO, floorVBO, floorEBO;

    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);

    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &floorEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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

    // Set polygon mode to line (wireframe)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

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

        // Set color blue
        glUseProgram(shaderProgramBlue);

        // Draw left wall
        glBindVertexArray(leftWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Draw right wall
        glBindVertexArray(rightWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Draw floor
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
