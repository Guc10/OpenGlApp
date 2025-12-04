#include "Boundaries.h"

Boundaries::Boundaries() {
    GLuint boundariesVBO, boundariesEBO;

    glGenVertexArrays(1, &boundariesVAO);
    glBindVertexArray(boundariesVAO);

    glGenBuffers(1, &boundariesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boundariesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &boundariesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundariesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}