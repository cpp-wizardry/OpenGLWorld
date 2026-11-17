#include "../Includes/Particle.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace {
    GLfloat RandRange(GLfloat lo, GLfloat hi) {
        GLfloat t = static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
        return lo + t * (hi - lo);
    }
    glm::vec3 RandRange(const glm::vec3& lo, const glm::vec3& hi) {
        return glm::vec3(RandRange(lo.x, hi.x), RandRange(lo.y, hi.y), RandRange(lo.z, hi.z));
    }
    constexpr GLfloat kTwoPi = 6.28318530718f;
}

ParticleSystem::ParticleSystem(size_t maxParticlesIn) : maxParticles(maxParticlesIn) {
    particles.reserve(maxParticles);
}

void ParticleSystem::SpawnOne(const glm::vec3& origin, const ParticleEmitConfig& config) {
    if (particles.size() >= maxParticles) return; 

    Particle p;
    p.position = origin;
    p.origin = origin;
    p.motion = config.motion;

    if (config.motion == ParticleMotion::Infinity) {
        p.patternRadius = config.patternRadius;
        p.patternSpeed = config.patternSpeed;
        p.patternAxisU = config.patternAxisU;
        p.patternAxisV = config.patternAxisV;
        p.patternPhase = config.randomizePhase ? RandRange(0.0f, kTwoPi) : 0.0f;
    }
    else {
        p.velocity = RandRange(config.velocityMin, config.velocityMax);
        p.gravity = config.gravity;
    }

    p.size = RandRange(config.sizeMin, config.sizeMax);
    p.maxLife = RandRange(config.lifeMin, config.lifeMax);
    p.life = p.maxLife;
    p.color = config.color + config.colorVariance * RandRange(glm::vec3(-1.0f), glm::vec3(1.0f));
    particles.push_back(p);
}

void ParticleSystem::Emit(const glm::vec3& origin, int count, const ParticleEmitConfig& config) {
    for (int i = 0; i < count; ++i) SpawnOne(origin, config);
    dirty = true;
}

void ParticleSystem::StartLoop(const glm::vec3& origin, GLfloat ratePerSecond, const ParticleEmitConfig& config) {
    looping = true;
    loopOrigin = origin;
    loopRate = ratePerSecond;
    loopConfig = config;
    loopAccumulator = 0.0f;
}

void ParticleSystem::StopLoop() {
    looping = false;
}

void ParticleSystem::Update(GLfloat deltaTime) {
    if (looping && loopRate > 0.0f) {
        loopAccumulator += deltaTime * loopRate;
        while (loopAccumulator >= 1.0f) {
            SpawnOne(loopOrigin, loopConfig);
            loopAccumulator -= 1.0f;
        }
    }

    for (auto& p : particles) {
        p.life -= deltaTime;
        p.age += deltaTime;

        if (p.motion == ParticleMotion::Infinity) {
            GLfloat t = p.patternPhase + p.age * p.patternSpeed;
            GLfloat s = std::sin(t);
            GLfloat denom = 1.0f + s * s;
            GLfloat u = p.patternRadius * std::cos(t) / denom;
            GLfloat v = p.patternRadius * s * std::cos(t) / denom;
            p.position = p.origin + p.patternAxisU * u + p.patternAxisV * v;
        }
        else {
            p.velocity += p.gravity * deltaTime;
            p.position += p.velocity * deltaTime;
        }
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );

    dirty = true;
}

void ParticleSystem::RebuildBuffer() {
    std::vector<GLfloat> interleaved;
    interleaved.reserve(particles.size() * 8);
    for (auto& p : particles) {
        GLfloat fade = glm::clamp(p.life / p.maxLife, 0.0f, 1.0f);
        interleaved.push_back(p.position.x);
        interleaved.push_back(p.position.y);
        interleaved.push_back(p.position.z);
        interleaved.push_back(p.color.r);
        interleaved.push_back(p.color.g);
        interleaved.push_back(p.color.b);
        interleaved.push_back(fade);
        interleaved.push_back(p.size);
    }

    if (interleaved.empty()) {
        dirty = false;
        return;
    }

    if (!mesh.IsValid()) {
        mesh = Mesh(interleaved, { {0, 3}, {1, 3}, {2, 1}, {3, 1} }, GL_DYNAMIC_DRAW);
    }
    else {
        mesh.UpdateVertices(interleaved);
    }
    dirty = false;
}

void ParticleSystem::Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    if (dirty) RebuildBuffer();
    if (particles.empty() || !mesh.IsValid()) return;

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    bool useTexture = (textureID != 0);
    glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), useTexture ? 1 : 0);
    if (useTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shader.ID, "particleTexture"), 0);
    }

    mesh.Draw(GL_POINTS, false);
}

void ParticleSystem::Clear() {
    particles.clear();
    dirty = true;
}