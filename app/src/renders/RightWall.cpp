#include "RightWall.h"

float rightWallVertices[] = {
    0.9f,  1.0f, 0.0f,  // top right
    0.9f, -1.0f, 0.0f,  // bottom right
    1.0f, -1.0f, 0.0f,  // bottom left
    1.0f,  1.0f, 0.0f   // top left
};

unsigned int rightWallIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

GLuint rightWallVAO;

void createRightWall() {
    GLuint rightWallVBO, rightWallEBO;

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
}
