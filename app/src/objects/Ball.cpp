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
    // Czyszcenie pamięci
    if (bouncingBallVBO) {
        glDeleteBuffers(1, &bouncingBallVBO);
    }
    if (bouncingBallVAO) {
        glDeleteVertexArrays(1, &bouncingBallVAO);
    }
}

void Ball::LaunchFromDrag(const glm::vec2 &drag, float power) {
    const float sensitivity = 4.0f;
    Velocity = drag * (sensitivity * power);
}

void Ball::UpdatePosition(glm::vec2 movementVector, float deltaTime) {
    if (deltaTime <= 0.0f) return;

    // Grawitacja
    Velocity.y -= Gravity * deltaTime;
    Position += Velocity * deltaTime;

    // Opór powietrza
    const float airDamping = 0.9995f;
    Velocity *= airDamping;
}

void Ball::CreateBouncingBall() {
    // Utworzenie piłki
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

    if (bouncingBallVAO == 0) {
        glGenVertexArrays(1, &bouncingBallVAO);
    }
    if (bouncingBallVBO == 0) {
        glGenBuffers(1, &bouncingBallVBO);
    }

    glBindVertexArray(bouncingBallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bouncingBallVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Ball::UpdateMesh() {
    // Generowanie piłki na podstawie obecnej pozycji
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

    glBindBuffer(GL_ARRAY_BUFFER, bouncingBallVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
