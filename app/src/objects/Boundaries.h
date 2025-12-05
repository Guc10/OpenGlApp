#ifndef OPENGLAPP_BOUNDARIES_H
#define OPENGLAPP_BOUNDARIES_H

#include "../config.h"
#include "glm/glm.hpp"
#include "Ball.h"

class Boundaries {
public:
    Boundaries();
    ~Boundaries();

    void ResolveCollision(Ball &ball);

    GLuint boundariesVAO;
private:
    float vertices[24] = {
        -1.0f, -1.0f, 0.0f,
        -0.9f, -0.9f, 0.0f,
         1.0f,  1.0f, 0.0f,
         0.9f,  0.9f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -0.9f,  0.9f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.9f, -0.9f, 0.0f
    };
    unsigned int indices[24] = {
        0, 1, 5,
        0, 4, 5,
        0, 1, 7,
        0, 6, 7,
        2, 3, 4,
        2, 3, 6,
        3, 6, 7,
        3, 4, 5
    };

    float minX = -0.9f, maxX = 0.9f;
    float minY = -0.9f, maxY = 0.9f;
};

#endif //OPENGLAPP_BOUNDARIES_H