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

glm::vec2 ClosestPointOnSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
{
    glm::vec2 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    return a + t * ab;
}

void Boundaries::ResolveCollision(Ball &ball) {
    glm::vec2 pos = ball.GetPosition();
    glm::vec2 vel = ball.GetVelocity();
    float r = ball.GetRadius();
    float refl = ball.GetReflectance();

    // get vertices and indices
    std::vector<glm::vec2> verts;
    verts.reserve(trianglesCount);

    for (int i = 0; i < trianglesCount * 3; i += 3)
        verts.emplace_back(vertices[i], vertices[i+1]);

    std::vector<unsigned int> idx(indices, indices + 24);

    for (int i = 0; i < trianglesCount * 3; i += 3)
    {
        glm::vec2 a = verts[indices[i]];
        glm::vec2 b = verts[indices[i+1]];
        glm::vec2 c = verts[indices[i+2]];

        // Check all 3 edges
        glm::vec2 edgePts[3][2] = {{a,b}, {b,c}, {c,a}};

        for (int e = 0; e < 3; e++)
        {
            glm::vec2 p1 = edgePts[e][0];
            glm::vec2 p2 = edgePts[e][1];

            glm::vec2 closest = ClosestPointOnSegment(pos, p1, p2);
            glm::vec2 diff = pos - closest;
            float dist = glm::length(diff);

            if (dist < r)  // COLLISION
            {
                glm::vec2 normal = glm::normalize(diff);

                // Push ball out of wall
                pos = closest + normal * r;

                // Reflect velocity
                vel = vel - 2.0f * glm::dot(vel, normal) * normal;
                vel *= refl; // damping
            }
        }
    }

    ball.SetPosition(pos);
    ball.SetVelocity(vel);
}
