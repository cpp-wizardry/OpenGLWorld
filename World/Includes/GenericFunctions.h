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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window, float dt);
void processInputsObject(GLFWwindow* window, glm::vec3& objectPos, glm::vec3& objectRotation, float deltaTime);

void getPlaneData(int& height, int& width);
void GetPathPrompt(HWND window, OPEN_MODES filter);

