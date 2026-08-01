//AppCon.h
#pragma once
#include "3DModelLoader.h"
#include "../../Includes/PointBuilder.h"
#include "../../Includes/ColorPicker.h"
#include "../../Includes/Camera.h"
#include "../../Includes/AnimObject.h"
#include "../../Includes/Primitives.h"
#include <vector>
#include <string>
struct AppContext {

    Manager3D Mng3D;
    std::vector<Mesh> modelMeshes;
    GLuint texture = 0;
    std::string currentAudioPath;

    PointBuilder Pbuild;
    ColorPicker Colors;

    AnimatedObject AnimObj;

    Camera* camera = nullptr;
};