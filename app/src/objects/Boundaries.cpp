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

// Funkcja pomocnicza
glm::vec2 ClosestPointOnSegment(glm::vec2 p, glm::vec2 a, glm::vec2 b)
{
    glm::vec2 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    return a + t * ab;
}

// Obliczanie kolizji
void Boundaries::ResolveCollision(Ball& ball)
{
    glm::vec2 pos = ball.GetPosition();
    glm::vec2 vel = ball.GetVelocity();
    float r = ball.GetRadius();
    float refl = ball.GetReflectance();

    for (int i = 0; i < 27; i += 3)
    {
        glm::vec2 A(vertices[indices[i] * 3],     vertices[indices[i] * 3 + 1]);
        glm::vec2 B(vertices[indices[i+1] * 3],   vertices[indices[i+1] * 3 + 1]);
        glm::vec2 C(vertices[indices[i+2] * 3],   vertices[indices[i+2] * 3 + 1]);

        glm::vec2 edges[3][2] = {{A,B},{B,C},{C,A}};

        for (int e = 0; e < 3; e++)
        {
            glm::vec2 p = ClosestPointOnSegment(pos, edges[e][0], edges[e][1]);
            glm::vec2 diff = pos - p;
            float dist2 = glm::dot(diff, diff);

            if (dist2 <= r * r)
            {
                glm::vec2 normal = glm::normalize(diff);

                pos = p + normal * r;

                vel = vel - 2.0f * glm::dot(vel, normal) * normal;
                vel *= refl;
            }
        }
    }

    ball.SetPosition(pos);
    ball.SetVelocity(vel);
}

