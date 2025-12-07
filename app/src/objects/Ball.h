#ifndef OPENGLAPP_BALL_H
#define OPENGLAPP_BALL_H

#include "../config.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <cstddef>


class Ball {
public:
    Ball(const float reflectance, const float radius, const unsigned int num_segments);
    ~Ball();

    void LaunchFromDrag(const glm::vec2 &drag, float power = 1.0f);
    void UpdatePosition(glm::vec2 movementVector, float deltaTime);
    void UpdateMesh();

    void ResetPosition() { Position = StartPosition; Velocity = glm::vec2(0.0f); }

    void SetReflectance(const float reflectance) { Reflectance = reflectance; }
    void SetRadius(const float radius) { Radius = radius; }
    void SetGravity(const float gravity) { Gravity = gravity; }

    glm::vec2 GetPosition() const { return Position; }
    glm::vec2 GetVelocity() const { return Velocity; }
    float GetRadius() const { return Radius; }
    float GetReflectance() const { return Reflectance; }
    unsigned int GetVertexCount() const { return NumSegments + 2; }

    GLuint bouncingBallVAO = 0;

    void SetPosition(const glm::vec2 &pos) {
        Position = pos;
        Velocity = glm::vec2(0.0f, 0.0f);
        UpdateMesh();
    }
    void SetVelocity(const glm::vec2 &vel) { Velocity = vel; }


private:
    float Reflectance = 0.9f;
    float Radius = 0.1f;
    float Gravity = 9.81f;
    unsigned int NumSegments = 0;

    GLuint bouncingBallVBO = 0;

    glm::vec2 Velocity = glm::vec2(0.0f, 0.0f);
    glm::vec2 Position = glm::vec2(0.0f, 0.0f);
    const glm::vec2 StartPosition = glm::vec2(0.0f, 0.0f);

    void CreateBouncingBall();
};

#endif //OPENGLAPP_BALL_H
