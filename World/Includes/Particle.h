//Particle.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include "Shader.h"

enum class ParticleMotion {
    Physics, 
    Infinity  
};

struct Particle {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 gravity = glm::vec3(0.0f, -2.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    GLfloat size = 0.1f;
    GLfloat life = 0.0f;   
    GLfloat maxLife = 1.0f; 

    ParticleMotion motion = ParticleMotion::Physics;
    glm::vec3 origin = glm::vec3(0.0f);       
    GLfloat age = 0.0f;                      
    GLfloat patternSpeed = 1.0f;               
    GLfloat patternRadius = 1.0f;
    GLfloat patternPhase = 0.0f;               
    glm::vec3 patternAxisU = glm::vec3(1.0f, 0.0f, 0.0f); 
    glm::vec3 patternAxisV = glm::vec3(0.0f, 0.0f, 1.0f);
};

struct ParticleEmitConfig {
    glm::vec3 velocityMin = glm::vec3(-1.0f, 1.0f, -1.0f);
    glm::vec3 velocityMax = glm::vec3(1.0f, 3.0f, 1.0f);
    glm::vec3 gravity = glm::vec3(0.0f, -2.0f, 0.0f);

    ParticleMotion motion = ParticleMotion::Physics;
    GLfloat patternRadius = 1.0f;
    GLfloat patternSpeed = 2.0f;  
    glm::vec3 patternAxisU = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 patternAxisV = glm::vec3(0.0f, 0.0f, 1.0f); 
    bool randomizePhase = true; 

    glm::vec3 color = glm::vec3(1.0f, 0.6f, 0.2f);
    glm::vec3 colorVariance = glm::vec3(0.0f); 

    GLfloat sizeMin = 0.05f;
    GLfloat sizeMax = 0.15f;

    GLfloat lifeMin = 0.5f;
    GLfloat lifeMax = 1.5f;
};

class ParticleSystem {
public:
    ParticleSystem() = default;
    explicit ParticleSystem(size_t maxParticles);

    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;
    ParticleSystem(ParticleSystem&&) noexcept = default;
    ParticleSystem& operator=(ParticleSystem&&) noexcept = default;

    void Emit(const glm::vec3& origin, int count, const ParticleEmitConfig& config);

    void StartLoop(const glm::vec3& origin, GLfloat ratePerSecond, const ParticleEmitConfig& config);
    void StopLoop();
    bool IsLooping() const { return looping; }
    void SetLoopOrigin(const glm::vec3& origin) { loopOrigin = origin; }

    void SetTexture(GLuint textureID_) { textureID = textureID_; }
    GLuint GetTexture() const { return textureID; }

    void Update(GLfloat deltaTime);
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    size_t ParticleCount() const { return particles.size(); }
    void Clear();

private:
    void SpawnOne(const glm::vec3& origin, const ParticleEmitConfig& config);
    void RebuildBuffer();

    std::vector<Particle> particles;
    size_t maxParticles = 4000;

    bool looping = false;
    glm::vec3 loopOrigin = glm::vec3(0.0f);
    GLfloat loopRate = 0.0f;
    ParticleEmitConfig loopConfig;
    GLfloat loopAccumulator = 0.0f;

    GLuint textureID = 0;

    Mesh mesh;
    bool dirty = true;
};