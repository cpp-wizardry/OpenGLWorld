//GenericFunctions.h
#pragma once
#include "stdafx.h"

using std::string;
enum OPEN_MODES
{
	AUDIO,
	VIDEO,
	GENERAL_PURPOSE,
	OBJECT3D,
	CONFIG
};

void processInput(GLFWwindow* window, float dt);
void getPlaneData(int& height, int& width);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void processInputsObject(GLFWwindow* window, glm::vec3& objectPos, glm::vec3& objectRotation, float deltaTime);

void generatePlanePoints(float* planeVertices, float width, float height);

void dynamicPlaneDraw(float* planeVertices,size_t count, unsigned int& planeVAO, unsigned int& planeVBO);

void GetPathPrompt(HWND window, OPEN_MODES filter);

