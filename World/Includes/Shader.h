//Shader.h
#pragma once
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vPath, const char* fPath) {
        std::string vCode = readFile(vPath);
        std::string fCode = readFile(fPath);
        const char* vSrc = vCode.c_str();
        const char* fSrc = fCode.c_str();
       
        
        unsigned int vertex = compile(GL_VERTEX_SHADER, vSrc);
        //Log en cas d'erreur shader de merde
        //std::cout << vSrc << "vertex"<< std::endl;
        unsigned int fragment = compile(GL_FRAGMENT_SHADER, fSrc);
        //Log en cas d'erreur shader de merde
        //std::cout << fSrc << "fragment"<< std::endl;

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkLink(ID);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() const { glUseProgram(ID); }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setVec3(const std::string& name, const glm::vec3& v) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
    }

private:
    std::string readFile(const char* path) {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    unsigned int compile(unsigned int type, const char* src) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info[512];
            glGetShaderInfoLog(shader, 512, nullptr, info);
            std::cerr << "Shader compilation failed:\n" << info << std::endl;
        }
        return shader;
    }

    void checkLink(unsigned int program) {
        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char info[512];
            glGetProgramInfoLog(program, 512, nullptr, info);
            std::cerr << "Program linking failed:\n" << info << std::endl;
        }
    }
};