#include "Boundaries.h"

Boundaries::Boundaries() {
    GLuint boundariesVBO, boundariesEBO;

    glGenVertexArrays(1, &boundariesVAO);
    glBindVertexArray(boundariesVAO);

    glGenBuffers(1, &boundariesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boundariesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &boundariesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundariesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Boundaries::~Boundaries() {
    if (boundariesVAO) {
        glDeleteVertexArrays(1, &boundariesVAO);
    }
}

void Boundaries::ResolveCollision(Ball &ball) {
    glm::vec2 pos = ball.GetPosition();
    glm::vec2 vel = ball.GetVelocity();
    float r = ball.GetRadius();
    float refl = ball.GetReflectance();

    // X axis collisions
    if (pos.x - r < minX) {
        pos.x = minX + r;
        vel.x = -vel.x * refl;
        // small damping on Y as well to simulate energy loss
        vel.y *= refl;
    } else if (pos.x + r > maxX) {
        pos.x = maxX - r;
        vel.x = -vel.x * refl;
        vel.y *= refl;
    }

    // Y axis collisions
    if (pos.y - r < minY) {
        pos.y = minY + r;
        vel.y = -vel.y * refl;
        // horizontal damping on floor contact
        vel.x *= refl;
    } else if (pos.y + r > maxY) {
        pos.y = maxY - r;
        vel.y = -vel.y * refl;
        vel.x *= refl;
    }

    ball.SetPosition(pos);
    ball.SetVelocity(vel);
}
