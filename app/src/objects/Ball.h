#ifndef OPENGLAPP_BALL_H
#define OPENGLAPP_BALL_H

#include "../config.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <cstddef>


class Ball {
public:
    // Constructor / Destructor
    Ball(const float reflectance, const float radius, const unsigned int num_segments);
    ~Ball();

    // Launch the ball from a drag vector in NDC (\[-1,1\])
    void LaunchFromDrag(const glm::vec2 &drag, float power = 1.0f);
    // Per-frame update: movementVector (unused for physics here) and deltaTime in seconds
    void UpdatePosition(glm::vec2 movementVector, float deltaTime);
    void UpdateMesh();

    // Reset
    void ResetPosition() { Position = StartPosition; Velocity = glm::vec2(0.0f); }

    // Setters
    void SetReflectance(const float reflectance) { Reflectance = reflectance; }
    void SetRadius(const float radius) { Radius = radius; }
    void SetGravity(const float gravity) { Gravity = gravity; }

    // Getters
    glm::vec2 GetPosition() const { return Position; }
    glm::vec2 GetVelocity() const { return Velocity; }
    float GetRadius() const { return Radius; }
    float GetReflectance() const { return Reflectance; }
    unsigned int GetVertexCount() const { return NumSegments + 2; }

    // Low-level access for renderer
    GLuint bouncingBallVAO = 0;
    // Allow boundary resolver to set position/velocity
    void SetPosition(const glm::vec2 &pos) {
        Position = pos;
        Velocity = glm::vec2(0.0f, 0.0f);
        UpdateMesh();
    }
    void SetVelocity(const glm::vec2 &vel) { Velocity = vel; }


private:
    // Ball properties
    float Reflectance = 0.9f;
    float Radius = 0.1f;
    float Gravity = 9.81f;
    unsigned int NumSegments = 0;

    // persistent VBO to avoid creating a new buffer every frame
    GLuint bouncingBallVBO = 0;

    glm::vec2 Velocity = glm::vec2(0.0f, 0.0f);
    glm::vec2 Position = glm::vec2(0.0f, 0.0f);
    const glm::vec2 StartPosition = glm::vec2(0.0f, 0.0f);

    void CreateBouncingBall();
};

#endif //OPENGLAPP_BALL_H
