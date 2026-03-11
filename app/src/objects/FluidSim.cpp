#include "FluidSim.h"
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float FluidSim::Poly6(float r2) const {
  float h2 = h_ * h_;
  if (r2 >= h2)
    return 0.0f;
  float f = h2 - r2;
  return (4.0f / ((float)M_PI * std::pow(h_, 8.0f))) * f * f * f;
}

glm::vec2 FluidSim::SpikyGrad(glm::vec2 r_vec, float r_len) const {
  if (r_len <= 0.0f || r_len >= h_)
    return glm::vec2(0.0f);
  float f = h_ - r_len;
  float coeff = -(30.0f / ((float)M_PI * std::pow(h_, 5.0f))) * f * f;
  return coeff * (r_vec / r_len);
}

float FluidSim::ViscLaplacian(float r_len) const {
  if (r_len >= h_)
    return 0.0f;
  return (40.0f / ((float)M_PI * std::pow(h_, 5.0f))) * (h_ - r_len);
}

FluidSim::FluidSim() {
  particles_.reserve(maxParticles_);
  InitParticleGL();
  InitSceneGL();
}

FluidSim::~FluidSim() {
  if (particleVAO_)
    glDeleteVertexArrays(1, &particleVAO_);
  if (circleVBO_)
    glDeleteBuffers(1, &circleVBO_);
  if (instanceVBO_)
    glDeleteBuffers(1, &instanceVBO_);
  if (sceneVAO_)
    glDeleteVertexArrays(1, &sceneVAO_);
  if (sceneVBO_)
    glDeleteBuffers(1, &sceneVBO_);
}

void FluidSim::Reset() {
  particles_.clear();
  spawnTimer_ = 0.0f;
}

void FluidSim::Update(float dt) {
  if (!running_)
    return;
  SpawnParticles(dt);

  const int substeps = 4;
  const float sdt = std::min(dt, 0.016f) / substeps;
  for (int s = 0; s < substeps; ++s) {
    ComputeDensityPressure();
    ComputeForces();
    Integrate(sdt);

    ResolveParticleCollisions();

    EnforceBoundaries();
  }

  UpdateInstanceBuffer();
}

void FluidSim::ComputeDensityPressure() {
  for (auto &pi : particles_) {
    pi.density = 0.0f;
    for (const auto &pj : particles_) {
      glm::vec2 r = pi.pos - pj.pos;
      float r2 = glm::dot(r, r);
      pi.density += mass_ * Poly6(r2);
    }
    pi.density = std::max(pi.density, 0.001f);
    pi.pressure = gasConstant_ * (pi.density - restDensity_);
  }
}

void FluidSim::ComputeForces() {
  int n = (int)particles_.size();
  for (int i = 0; i < n; ++i) {
    glm::vec2 fp(0.0f), fv(0.0f);
    for (int j = 0; j < n; ++j) {
      if (i == j)
        continue;
      glm::vec2 r_vec = particles_[i].pos - particles_[j].pos;
      float r_len = glm::length(r_vec);
      if (r_len >= h_ || r_len < 1e-6f)
        continue;

      float avgP = (particles_[i].pressure + particles_[j].pressure) * 0.5f;
      fp += -mass_ * avgP / particles_[j].density * SpikyGrad(r_vec, r_len);

      fv += viscosity_ * mass_ * (particles_[j].vel - particles_[i].vel) /
            particles_[j].density * ViscLaplacian(r_len);
    }
    glm::vec2 fg(0.0f, -gravity_ * particles_[i].density);
    particles_[i].force = fp + fv + fg;
  }
}

void FluidSim::Integrate(float dt) {
  for (auto &p : particles_) {
    p.vel += dt * p.force / p.density;
    p.pos += dt * p.vel;
    p.vel *= 0.9998f;
  }
}

void FluidSim::EnforceBoundaries() {
  for (auto &p : particles_) {
    if (p.pos.x - renderRadius_ < wallL_) {
      p.pos.x = wallL_ + renderRadius_;
      p.vel.x = std::abs(p.vel.x) * restitution_;
    }
    if (p.pos.x + renderRadius_ > wallR_) {
      p.pos.x = wallR_ - renderRadius_;
      p.vel.x = -std::abs(p.vel.x) * restitution_;
    }
    if (p.pos.y - renderRadius_ < wallB_) {
      p.pos.y = wallB_ + renderRadius_;
      p.vel.y = std::abs(p.vel.y) * restitution_;
    }
    if (p.pos.y + renderRadius_ > wallT_) {
      p.pos.y = wallT_ - renderRadius_;
      p.vel.y = -std::abs(p.vel.y) * restitution_;
    }
    ResolveObstacle(p);
  }
}

glm::vec2 FluidSim::ClosestOnSegment(glm::vec2 p, glm::vec2 a,
                                     glm::vec2 b) const {
  glm::vec2 ab = b - a;
  float t = glm::clamp(glm::dot(p - a, ab) / glm::dot(ab, ab), 0.0f, 1.0f);
  return a + t * ab;
}

void FluidSim::ResolveObstacle(Particle &p) {
  glm::vec2 edges[3][2] = {
      {obstA_, obstB_}, {obstB_, obstC_}, {obstC_, obstA_}};
  for (auto &edge : edges) {
    glm::vec2 closest = ClosestOnSegment(p.pos, edge[0], edge[1]);
    glm::vec2 diff = p.pos - closest;
    float dist = glm::length(diff);
    if (dist < renderRadius_ * 1.5f && dist > 1e-6f) {
      glm::vec2 n = diff / dist;
      p.pos = closest + n * (renderRadius_ * 1.5f);
      float vn = glm::dot(p.vel, n);
      if (vn < 0.0f)
        p.vel -= (1.0f + restitution_) * vn * n;
    }
  }
}

void FluidSim::ResolveParticleCollisions() {
  int n = (int)particles_.size();

  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {

      glm::vec2 r = particles_[i].pos - particles_[j].pos;
      float dist = glm::length(r);
      float minDist = 2.0f * particleRadius_;

      if (dist < minDist && dist > 1e-6f) {
        glm::vec2 normal = r / dist;
        float penetration = minDist - dist;

        particles_[i].pos += 0.5f * penetration * normal;
        particles_[j].pos -= 0.5f * penetration * normal;

        float vi = glm::dot(particles_[i].vel, normal);
        float vj = glm::dot(particles_[j].vel, normal);

        float impulse = (vi - vj) * 0.5f;

        particles_[i].vel -= impulse * normal;
        particles_[j].vel += impulse * normal;
      }
    }
  }
}

void FluidSim::SpawnParticles(float dt) {
  if ((int)particles_.size() >= maxParticles_)
    return;
  spawnTimer_ += dt;
  if (spawnTimer_ < spawnInterval_)
    return;
  spawnTimer_ = 0.0f;

  static std::mt19937 rng(42);
  std::uniform_real_distribution<float> distX(-0.06f, 0.06f);
  std::uniform_real_distribution<float> distVX(-0.15f, 0.15f);

  int toSpawn = std::min(quality_, maxParticles_ - (int)particles_.size());
  for (int i = 0; i < toSpawn; ++i) {
    Particle np;
    np.pos = {distX(rng), 0.72f};
    np.vel = {distVX(rng), -0.4f};
    np.force = glm::vec2(0.0f);
    particles_.push_back(np);
  }
}

void FluidSim::InitParticleGL() {
  const int segs = 14;
  circleVerts_ = segs + 2;

  std::vector<glm::vec2> circle;
  circle.reserve(circleVerts_);
  circle.push_back({0.0f, 0.0f});
  for (int i = 0; i <= segs; ++i) {
    float theta = 2.0f * (float)M_PI * i / segs;
    circle.push_back({std::cos(theta), std::sin(theta)});
  }

  glGenVertexArrays(1, &particleVAO_);
  glBindVertexArray(particleVAO_);

  glGenBuffers(1, &circleVBO_);
  glBindBuffer(GL_ARRAY_BUFFER, circleVBO_);
  glBufferData(GL_ARRAY_BUFFER, circle.size() * sizeof(glm::vec2),
               circle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribDivisor(0, 0);

  glGenBuffers(1, &instanceVBO_);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_);
  glBufferData(GL_ARRAY_BUFFER, maxParticles_ * sizeof(glm::vec3), nullptr,
               GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribDivisor(1, 1);

  glBindVertexArray(0);
}

void FluidSim::UpdateInstanceBuffer() {
  int n = (int)particles_.size();
  if (n == 0)
    return;

  float maxSpeed = 0.1f;
  for (const auto &p : particles_) {
    float s = glm::length(p.vel);
    if (s > maxSpeed)
      maxSpeed = s;
  }

  std::vector<glm::vec3> inst(n);
  for (int i = 0; i < n; ++i) {
    float speed01 =
        glm::clamp(glm::length(particles_[i].vel) / maxSpeed, 0.0f, 1.0f);
    inst[i] = {particles_[i].pos.x, particles_[i].pos.y, speed01};
  }

  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, n * sizeof(glm::vec3), inst.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FluidSim::RenderParticles(GLuint program, GLint uRadius, GLint uColorLow,
                               GLint uColorHigh) {
  int n = (int)particles_.size();
  if (n == 0)
    return;

  glUseProgram(program);
  if (uRadius >= 0)
    glUniform1f(uRadius, renderRadius_);
  if (uColorLow >= 0)
    glUniform3fv(uColorLow, 1, &baseColor_[0]);
  glm::vec3 highColor =
      glm::mix(baseColor_, glm::vec3(1.0f, 0.95f, 0.85f), 0.85f);
  if (uColorHigh >= 0)
    glUniform3fv(uColorHigh, 1, &highColor[0]);

  glBindVertexArray(particleVAO_);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, circleVerts_, n);
  glDisable(GL_BLEND);
  glBindVertexArray(0);
  glUseProgram(0);
}

void FluidSim::InitSceneGL() {
  const float il = wallL_, ir = wallR_, ib = wallB_, it = wallT_;
  const float tw = 0.05f; // wall thickness

  auto pushQuad = [](std::vector<float> &v, float x0, float y0, float x1,
                     float y1) {
    v.insert(v.end(), {x0, y0, x1, y0, x1, y1, x0, y0, x1, y1, x0, y1});
  };

  std::vector<float> verts;
  verts.reserve((24 + 3 + 6) * 2);

  pushQuad(verts, il - tw, ib - tw, il, it + tw);
  pushQuad(verts, ir, ib - tw, ir + tw, it + tw);
  pushQuad(verts, il - tw, ib - tw, ir + tw, ib);
  pushQuad(verts, il - tw, it, ir + tw, it + tw);

  verts.insert(verts.end(),
               {obstA_.x, obstA_.y, obstB_.x, obstB_.y, obstC_.x, obstC_.y});

  pushQuad(verts, -0.09f, 0.78f, 0.09f, 0.88f);

  glGenVertexArrays(1, &sceneVAO_);
  glBindVertexArray(sceneVAO_);

  glGenBuffers(1, &sceneVBO_);
  glBindBuffer(GL_ARRAY_BUFFER, sceneVBO_);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void FluidSim::RenderScene(GLuint program, GLint uColor) {
  glUseProgram(program);
  glBindVertexArray(sceneVAO_);

  if (uColor >= 0)
    glUniform4f(uColor, 0.25f, 0.28f, 0.33f, 1.0f);
  glDrawArrays(GL_TRIANGLES, 0, 24);

  if (uColor >= 0)
    glUniform4f(uColor, 0.42f, 0.45f, 0.52f, 1.0f);
  glDrawArrays(GL_TRIANGLES, 24, 3);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (uColor >= 0)
    glUniform4f(uColor, 0.10f, 0.85f, 0.75f, 0.55f);
  glDrawArrays(GL_TRIANGLES, 27, 6);
  glDisable(GL_BLEND);

  glBindVertexArray(0);
  glUseProgram(0);
}