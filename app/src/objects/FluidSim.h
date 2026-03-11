#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Particle {
  glm::vec2 pos;
  glm::vec2 vel;
  glm::vec2 force;
  float density = 0.0f;
  float pressure = 0.0f;
};

class FluidSim {
public:
  FluidSim();
  ~FluidSim();

  void Update(float dt);
  void RenderParticles(GLuint program, GLint uRadius, GLint uColorLow,
                       GLint uColorHigh);
  void RenderScene(GLuint program, GLint uColor);

  void SetGravity(float g) { gravity_ = g; }
  void SetViscosity(float v) { viscosity_ = glm::clamp(v, 0.0f, 10.0f); }
  void SetQuality(int q) { quality_ = glm::clamp(q, 1, 10); }
  void SetRenderRadius(float r) {
    renderRadius_ = r;
    particleRadius_ = r;
  }
  void SetBaseColor(glm::vec3 c) { baseColor_ = c; }
  void SetRunning(bool r) { running_ = r; }
  void Reset();

  int GetParticleCount() const { return (int)particles_.size(); }
  float GetViscosity() const { return viscosity_; }
  float GetGravity() const { return gravity_; }

private:
  std::vector<Particle> particles_;

  float particleRadius_ = 0.022f;
  void ResolveParticleCollisions();

  // ---- SPH parameters ----
  float h_ = 0.055f;
  float mass_ = 1.0f;
  float restDensity_ = 25.0f;
  float gasConstant_ = 90.0f;
  float viscosity_ = 1.2f;
  float gravity_ = 2.5f;

  float renderRadius_ = 0.022f;
  glm::vec3 baseColor_ = {0.15f, 0.55f, 1.0f};

  bool running_ = true;
  int quality_ = 1;
  float spawnTimer_ = 0.0f;
  static constexpr float spawnInterval_ = 0.3f;
  static constexpr int maxParticles_ = 550;

  static constexpr float wallL_ = -0.85f;
  static constexpr float wallR_ = 0.85f;
  static constexpr float wallB_ = -0.85f;
  static constexpr float wallT_ = 0.85f;
  static constexpr float restitution_ = 0.2f;

  glm::vec2 obstA_ = {-0.28f, -0.85f};
  glm::vec2 obstB_ = {0.28f, -0.85f};
  glm::vec2 obstC_ = {0.00f, -0.46f};

  GLuint particleVAO_ = 0;
  GLuint circleVBO_ = 0;
  GLuint instanceVBO_ = 0;
  int circleVerts_ = 0;

  GLuint sceneVAO_ = 0;
  GLuint sceneVBO_ = 0;

  float Poly6(float r2) const;
  glm::vec2 SpikyGrad(glm::vec2 r_vec, float r_len) const;
  float ViscLaplacian(float r_len) const;

  void ComputeDensityPressure();
  void ComputeForces();
  void Integrate(float dt);
  void EnforceBoundaries();
  void SpawnParticles(float dt);
  void ResolveObstacle(Particle &p);
  glm::vec2 ClosestOnSegment(glm::vec2 p, glm::vec2 a, glm::vec2 b) const;

  void InitParticleGL();
  void InitSceneGL();
  void UpdateInstanceBuffer();
};
