#include "Ball.h"

Ball::Ball(const float reflectance, const float radius, const unsigned int num_segments) {
    Reflectance = reflectance;
    Radius = radius;
    NumSegments = num_segments;
    CreateBouncingBall();
}


void Ball::CreateBouncingBall() {
    float timeValue = glfwGetTime();
    float vertices[2 * (NumSegments + 2)];
    float cx = sin(timeValue)/2, cy = cos(timeValue)/2, r = Radius;
    vertices[0] = cx;
    vertices[1] = cy;
    for (int i = 0; i <= NumSegments; ++i) {
        float theta = 2.0f * M_PIf * float(i) / float(NumSegments);
        vertices[2 * (i + 1)] = cx + r * cosf(theta);
        vertices[2 * (i + 1) + 1] = cy + r * sinf(theta);
    }

    GLuint bouncingBallVBO;

    glGenVertexArrays(1, &bouncingBallVAO);
    glBindVertexArray(bouncingBallVAO);

    glGenBuffers(1, &bouncingBallVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bouncingBallVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
}
