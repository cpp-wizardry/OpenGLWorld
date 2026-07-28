//Camera.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw = -90.0f;
    float Pitch = 0.0f;
    float Speed = 5.0f;
    float Sensitivity = 0.1f;
    float Zoom = 45.0f;
    float BaseFOV = 45.0f;
    float MoveFOV = 65.0f;
    float FOVLerpSpeed = 8.0f;


    Camera(glm::vec3 position = glm::vec3(0.0f, 2.0f, 5.0f)) : Position(position), WorldUp(0, 1, 0) {
        updateVectors();
    }

    glm::mat4 GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }

    void ProcessMouse(float xoffset, float yoffset) {
        Yaw += xoffset * Sensitivity;
        Pitch += yoffset * Sensitivity;
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
        updateVectors();
    }

    void MoveForward(float deltaTime) { Position += Front * Speed * deltaTime; }
    void MoveBackward(float deltaTime) { Position -= Front * Speed * deltaTime; }
    void MoveLeft(float deltaTime) { Position -= Right * Speed * deltaTime; }
    void MoveRight(float deltaTime) { Position += Right * Speed * deltaTime; }
    void UpdateFOV(bool isMoving, float dt) {
        float target = isMoving ? MoveFOV : BaseFOV;
        float t = std::min(1.0f, FOVLerpSpeed * dt);
        Zoom += (target - Zoom) * t;
    }
private:
    void updateVectors() {
        Front = glm::normalize(glm::vec3(
            cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
            sin(glm::radians(Pitch)),
            sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))
        ));
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};