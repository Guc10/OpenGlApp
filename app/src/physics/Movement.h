#ifndef OPENGLAPP_MOVEMENT_H
#define OPENGLAPP_MOVEMENT_H

#include "glm/vec2.hpp"

class Movement {
public:
    static glm::vec2 getMousePosition(float xpos, float ypos);
    static glm::vec2 calculateVector(glm::vec2 currentBallPosition, glm::vec2 mousePosition);
};


#endif //OPENGLAPP_MOVEMENT_H