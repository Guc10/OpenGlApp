#ifndef OPENGLAPP_MOVEMENT_H
#define OPENGLAPP_MOVEMENT_H

#include "glm/vec2.hpp"

class Movement {
public:
    static glm::vec2 getMousevector(float xpos, float ypos, glm::vec2 &currentBallPosition);
};

#endif //OPENGLAPP_MOVEMENT_H