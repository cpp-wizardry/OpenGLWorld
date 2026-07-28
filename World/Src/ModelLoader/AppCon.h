//AppCon.h
#pragma once
#include "3DModelLoader.h"
#include "../../Includes/PointBuilder.h"

struct AppContext {    
    
    Manager3D Mng3D;
    std::vector<GLuint> VBOs; //changed it so I can have multiple independent objects
    std::vector<GLuint> VAOs;
    GLuint texture = 0;
    std::vector<GLsizei> vertexCounts;
    //AudioManager* audio;
    std::string currentAudioPath;
    PointBuilder Pbuild;
};
