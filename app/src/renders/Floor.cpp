#include "Floor.h"

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

GLuint floorVAO;

void createFloor() {
    GLuint floorVBO, floorEBO;

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
}