#include "../Includes/Animator.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

glm::mat4 Pose::GetModelMatrix() const {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(rotation);
    model = glm::scale(model, scale);
    return model;
}

void Animator::SetBasePosition(const glm::vec3& pos) {
    basePosition = pos;
    pose.position = pos;
}

GLfloat Animator::ApplyEase(GLfloat t, EaseType ease) {
    t = glm::clamp(t, 0.0f, 1.0f);
    if (ease == EaseType::EaseInOut) {
        return t * t * (3.0f - 2.0f * t);
    }
    return t;
}

void Animator::Rotate(const glm::vec3& axis, GLfloat degreesPerSecond) {
    if (glm::length(axis) < 1e-6f) return;
    rotateAxis = glm::normalize(axis);
    rotateDegPerSec = degreesPerSecond;
    rotating = true;
}

void Animator::StopRotate() {
    rotating = false;
    rotateDegPerSec = 0.0f;
}

void Animator::MoveTo(const glm::vec3& targetPosition, GLfloat duration, EaseType ease) {
    moveStart = pose.position;
    moveTarget = targetPosition;
    moveDuration = std::max(duration, 0.0001f);
    moveElapsed = 0.0f;
    moveEase = ease;
    moving = true;
}

void Animator::Vibrate(GLfloat amplitude, GLfloat frequency, GLfloat duration) {
    vibrateAmplitude = amplitude;
    vibrateFrequency = frequency;
    vibrateDuration = duration;
    vibrateElapsed = 0.0f;
    vibrating = true;
}

void Animator::StopVibrate() {
    vibrating = false;
    if (!moving) pose.position = basePosition;
}

void Animator::Spin(const glm::vec3& axis, GLfloat spinCount, GLfloat duration, EaseType ease) {
    if (glm::length(axis) < 1e-6f) return;
    spinAxis = glm::normalize(axis);
    spinTotalDegrees = 360.0f * spinCount;
    spinDuration = std::max(duration, 0.0001f);
    spinElapsed = 0.0f;
    spinLastAngle = 0.0f;
    spinEase = ease;
    spinning = true;
}

void Animator::Update(GLfloat deltaTime) {
    if (rotating && std::fabs(rotateDegPerSec) > 0.0f) {
        GLfloat deltaDeg = rotateDegPerSec * deltaTime;
        glm::quat delta = glm::angleAxis(glm::radians(deltaDeg), rotateAxis);
        pose.rotation = glm::normalize(delta * pose.rotation);
    }

    if (spinning) {
        spinElapsed += deltaTime;
        GLfloat t = spinElapsed / spinDuration;
        GLfloat easedT = ApplyEase(std::min(t, 1.0f), spinEase);
        GLfloat targetAngle = spinTotalDegrees * easedT;
        GLfloat deltaAngle = targetAngle - spinLastAngle;
        spinLastAngle = targetAngle;

        glm::quat delta = glm::angleAxis(glm::radians(deltaAngle), spinAxis);
        pose.rotation = glm::normalize(delta * pose.rotation);

        if (t >= 1.0f) {
            spinning = false;
        }
    }

    if (moving) {
        moveElapsed += deltaTime;
        GLfloat t = ApplyEase(moveElapsed / moveDuration, moveEase);
        pose.position = glm::mix(moveStart, moveTarget, t);
        basePosition = pose.position;

        if (moveElapsed >= moveDuration) {
            pose.position = moveTarget;
            basePosition = moveTarget;
            moving = false;
        }
    }

    if (vibrating) {
        vibrateElapsed += deltaTime;
        if (vibrateDuration > 0.0f && vibrateElapsed >= vibrateDuration) {
            vibrating = false;
            if (!moving) pose.position = basePosition;
        }
        else {
            GLfloat phase = vibrateElapsed * vibrateFrequency * 2.0f * glm::pi<GLfloat>();
            glm::vec3 offset(
                std::sin(phase * 1.0f),
                std::sin(phase * 1.3f + 1.5f),
                std::sin(phase * 0.7f + 3.0f)
            );
            pose.position = basePosition + offset * vibrateAmplitude;
        }
    }
}