#include "Movement.h"

glm::vec2 Movement::getMousevector(float xpos, float ypos, glm::vec2 &currentBallPosition) {
    return glm::vec2(xpos, ypos) - currentBallPosition;
}
