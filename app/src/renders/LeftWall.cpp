#include "LeftWall.h"

float leftWallVertices[] = {
    -0.9f,  1.0f, 0.0f, // top right
    -0.9f, -1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f, // bottom left
    -1.0f,  1.0f, 0.0f  // top left
};

unsigned int leftWallIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

GLuint leftWallVAO;

void createLeftWall() {
    GLuint leftWallVBO, leftWallEBO;

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
}