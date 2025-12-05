#include "Ball.h"
#include <glad/glad.h>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ball::Ball(const float reflectance, const float radius, const unsigned int num_segments) {
    Reflectance = reflectance;
    Radius = radius;
    NumSegments = num_segments > 2 ? num_segments : 20;
    Position = StartPosition;
    Velocity = glm::vec2(0.0f);
    bouncingBallVAO = 0;
    bouncingBallVBO = 0;
    CreateBouncingBall();
}

Ball::~Ball() {
    // delete VAO/VBO when destructed
    if (bouncingBallVBO) {
        glDeleteBuffers(1, &bouncingBallVBO);
    }
    if (bouncingBallVAO) {
        glDeleteVertexArrays(1, &bouncingBallVAO);
    }
}

void Ball::LaunchFromDrag(const glm::vec2 &drag, float power) {
    // drag is expected in NDC (end - start). Positive Y is up.
    const float sensitivity = 4.0f; // tweak as needed
    Velocity = drag * (sensitivity * power);
}

void Ball::UpdatePosition(glm::vec2 movementVector, float deltaTime) {
    if (deltaTime <= 0.0f) return;

    // Apply gravity (downwards in NDC -> negative Y)
    Velocity.y -= Gravity * deltaTime;

    // Integrate position
    Position += Velocity * deltaTime;

    // Simple air damping to avoid perpetual motion
    const float airDamping = 0.9995f;
    Velocity *= airDamping;
}

void Ball::CreateBouncingBall() {
    // build vertex array for circle (center + ring)
    std::vector<float> vertices;
    vertices.reserve(2 * (NumSegments + 2));
    float cx = Position.x;
    float cy = Position.y;
    vertices.push_back(cx);
    vertices.push_back(cy);
    for (unsigned int i = 0; i <= NumSegments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)NumSegments;
        vertices.push_back(cx + Radius * cosf(theta));
        vertices.push_back(cy + Radius * sinf(theta));
    }

    // ensure VAO/VBO exist (generate once)
    if (bouncingBallVAO == 0) {
        glGenVertexArrays(1, &bouncingBallVAO);
    }
    if (bouncingBallVBO == 0) {
        glGenBuffers(1, &bouncingBallVBO);
    }

    // bind and upload vertex data
    glBindVertexArray(bouncingBallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bouncingBallVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind to be tidy
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Ball::UpdateMesh() {
    // build vertices centered at current Position
    std::vector<float> vertices;
    vertices.reserve(2 * (NumSegments + 2));
    float cx = Position.x;
    float cy = Position.y;
    vertices.push_back(cx);
    vertices.push_back(cy);
    for (unsigned int i = 0; i <= NumSegments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)NumSegments;
        vertices.push_back(cx + Radius * cosf(theta));
        vertices.push_back(cy + Radius * sinf(theta));
    }

    // upload to GPU (replace buffer contents)
    glBindBuffer(GL_ARRAY_BUFFER, bouncingBallVBO);
    // replace data (size matches allocation in CreateBouncingBall)
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
