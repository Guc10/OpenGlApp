//
// Created by guc10 on 3.12.2025.
//

#ifndef OPENGLAPP_BALL_H
#define OPENGLAPP_BALL_H

#include "glm/glm.hpp"

class Ball {
public:
    // Constructor
    explicit Ball(const float reflectance, const float radius): Reflectance(reflectance), Radius(radius) {};
    ~Ball();

    // Update ball position based on movement vector and delta time or reset to start position
    void UpdatePosition(glm::vec2 movementVector, float deltaTime);
    void ResetPosition() { Position = StartPosition; };

    // Setters
    void SetReflectance(float reflectance) { Reflectance = reflectance; }
    void SetRadius(float radius) { Radius = radius; }
    void SetGravity(float gravity) { Gravity = gravity; }

    // Getters
    glm::vec2 GetPosition() const { return Position; }
private:
    // Ball properties
    float Reflectance = 0;
    float Radius = 0;
    float Gravity = 9.81f;

    glm::vec2 Velocity = glm::vec2(0.0f, 0.0f);

    glm::vec2 Position = glm::vec2(0.0f, 0.0f);

    const glm::vec2 StartPosition = glm::vec2(0.0f, 0.0f);
};


#endif //OPENGLAPP_BALL_H