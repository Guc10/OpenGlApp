#ifndef OPENGLAPP_BOUNDARIES_H
#define OPENGLAPP_BOUNDARIES_H

#include "../config.h"
#include "glm/glm.hpp"
#include "Ball.h"

glm::vec2 ClosestPointOnSegment(glm::vec2 p, glm::vec2 a, glm::vec2 b);

class Boundaries {
public:
    Boundaries();
    ~Boundaries();

    void ResolveCollision(Ball &ball);


    GLuint boundariesVAO;
private:
    float vertices[30] = {
        -1.0f, -1.0f, 0.0f,
        -0.9f, -0.9f, 0.0f,
         1.0f,  1.0f, 0.0f,
         0.9f,  0.9f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -0.9f,  0.9f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.9f, -0.9f, 0.0f,
         0.7f, -0.5f, 0.0f,
         0.0f, -0.9f, 0.0f,
    };
    unsigned int indices[30] = {
        0, 1, 5,
        0, 4, 5,
        0, 1, 7,
        0, 6, 7,
        2, 3, 4,
        2, 3, 6,
        3, 6, 7,
        3, 4, 5,
        9, 7, 8
    };
};

#endif //OPENGLAPP_BOUNDARIES_H