//Animation.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class EaseType {
    Linear,
    EaseInOut
};

struct Pose {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); 
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix() const;
};

class Animator
{
public:
    Animator() = default;

    void SetBasePosition(const glm::vec3& pos);
    void SetScale(const glm::vec3& newScale) { pose.scale = newScale; }


    void Rotate(const glm::vec3& axis, GLfloat degreesPerSecond);
    void StopRotate();
    void MoveTo(const glm::vec3& targetPosition, GLfloat duration, EaseType ease = EaseType::EaseInOut);
    void Transform(const glm::vec3& targetPosition, GLfloat duration, EaseType ease = EaseType::EaseInOut) {
        MoveTo(targetPosition, duration, ease);
    }

    void Vibrate(GLfloat amplitude, GLfloat frequency, GLfloat duration = 0.0f);
    void StopVibrate();

    void Spin(const glm::vec3& axis, GLfloat spinCount, GLfloat duration, EaseType ease = EaseType::Linear);

    void Update(GLfloat deltaTime);

    const Pose& GetPose() const { return pose; }
    bool IsMoving() const { return moving; }
    bool IsSpinning() const { return spinning; }
    bool IsVibrating() const { return vibrating; }
    bool IsRotating() const { return rotating; }

private:
    static GLfloat ApplyEase(GLfloat t, EaseType ease);

    Pose pose;
    glm::vec3 basePosition = glm::vec3(0.0f);

    bool rotating = false;
    glm::vec3 rotateAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    GLfloat rotateDegPerSec = 0.0f;

    bool moving = false;
    glm::vec3 moveStart = glm::vec3(0.0f);
    glm::vec3 moveTarget = glm::vec3(0.0f);
    GLfloat moveDuration = 0.0f;
    GLfloat moveElapsed = 0.0f;
    EaseType moveEase = EaseType::EaseInOut;

    bool vibrating = false;
    GLfloat vibrateAmplitude = 0.0f;
    GLfloat vibrateFrequency = 0.0f;
    GLfloat vibrateDuration = 0.0f;
    GLfloat vibrateElapsed = 0.0f;

    bool spinning = false;
    glm::vec3 spinAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    GLfloat spinTotalDegrees = 0.0f;
    GLfloat spinDuration = 0.0f;
    GLfloat spinElapsed = 0.0f;
    GLfloat spinLastAngle = 0.0f;
    EaseType spinEase = EaseType::Linear;
}; 