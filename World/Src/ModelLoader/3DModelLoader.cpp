//3DModelLoader.cpp
#include "3DModelLoader.h"


std::vector<MeshData> Manager3D::loadOBJ(const std::string& Path) {
    std::vector<MeshData> meshes;
    MeshData currentMesh;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    std::ifstream file(Path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << Path << "\n";
        return {};
    }

    std::string line;

    auto addVertex = [&](GLint vi, GLint ti, GLint ni) {
        glm::vec3 pos(0.0f), norm(0.0f);
        glm::vec2 tex(0.0f);

        if (vi > 0 && vi <= (GLint)positions.size())
            pos = positions[vi - 1];
        if (ti > 0 && ti <= (GLint)texcoords.size())
            tex = texcoords[ti - 1];
        if (ni > 0 && ni <= (GLint)normals.size())
            norm = normals[ni - 1];

        currentMesh.vertices.insert(currentMesh.vertices.end(),
            { pos.x, pos.y, pos.z,
              tex.x, tex.y,
              norm.x, norm.y, norm.z });
        };

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "o") {
            if (!currentMesh.vertices.empty()) {
                meshes.push_back(currentMesh);
                currentMesh.vertices.clear();
            }
        }
        else if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::vector<GLint> vIdx, tIdx, nIdx;
            std::stringstream ss(line.substr(2));
            std::string vertex;

            while (ss >> vertex) {
                GLint vi = 0, ti = 0, ni = 0;
                if (sscanf_s(vertex.c_str(), "%d/%d/%d", &vi, &ti, &ni) == 3) {
                }
                else if (sscanf_s(vertex.c_str(), "%d//%d", &vi, &ni) == 2) {
                }
                else if (sscanf_s(vertex.c_str(), "%d/%d", &vi, &ti) == 2) {

                }
                else {
                    std::cout << sscanf_s(vertex.c_str(), "%d", &vi);
                }

                vIdx.push_back(vi);
                tIdx.push_back(ti);
                nIdx.push_back(ni);
            }

            if (vIdx.size() == 3) {
                addVertex(vIdx[0], tIdx[0], nIdx[0]);
                addVertex(vIdx[1], tIdx[1], nIdx[1]);
                addVertex(vIdx[2], tIdx[2], nIdx[2]);
            }
            else if (vIdx.size() == 4) {
                addVertex(vIdx[0], tIdx[0], nIdx[0]);
                addVertex(vIdx[1], tIdx[1], nIdx[1]);
                addVertex(vIdx[2], tIdx[2], nIdx[2]);

                addVertex(vIdx[0], tIdx[0], nIdx[0]);
                addVertex(vIdx[2], tIdx[2], nIdx[2]);
                addVertex(vIdx[3], tIdx[3], nIdx[3]);
            }
            else if (vIdx.size() > 4) {
                for (GLsizei i = 1; i + 1 < vIdx.size(); i++) {
                    addVertex(vIdx[0], tIdx[0], nIdx[0]);
                    addVertex(vIdx[i], tIdx[i], nIdx[i]);
                    addVertex(vIdx[i + 1], tIdx[i + 1], nIdx[i + 1]);
                }
            }
        }
    }

    if (!currentMesh.vertices.empty())
        meshes.push_back(currentMesh);

    return meshes;
}



void Manager3D::centerAndNormalizeOBJ(std::vector<GLfloat>& vertices) {
    if (vertices.empty()) return;

    glm::vec3 mini(FLT_MAX);
   
    glm::vec3 maxi(-FLT_MAX);

    for (GLsizei i = 0; i < vertices.size(); i += 8) {
        glm::vec3 pos(vertices[i], vertices[i + 1], vertices[i + 2]);

        mini.x = min(mini.x, pos.x);
        mini.y = min(mini.y, pos.y);
        mini.z = min(mini.z, pos.z);

        maxi.x = max(maxi.x, pos.x);
        maxi.y = max(maxi.y, pos.y);
        maxi.z = max(maxi.z, pos.z);
    }

    glm::vec3 center = (mini + maxi) / 2.0f;
    GLfloat maxExtent = max( maxi.x - mini.x, maxi.y - mini.y, maxi.z - mini.z);


    for (size_t i = 0; i < vertices.size(); i += 8) {
        vertices[i + 0] = (vertices[i + 0] - center.x) / maxExtent;
        vertices[i + 1] = (vertices[i + 1] - center.y) / maxExtent;
        vertices[i + 2] = (vertices[i + 2] - center.z) / maxExtent;
    }
}



std::vector<GLsizei> Manager3D::reloadModel(const std::string& path,std::vector<GLuint>& VBOs,std::vector<GLuint>& VAOs)
{
    if (path.empty()) return {};

    std::vector<MeshData> newMeshes = loadOBJ(path);
    if (newMeshes.empty()) {
        std::cerr << "reloadModel: failed to load OBJ: " << path << "\n";
        return {};
    }


    if (!VBOs.empty()) {
        glDeleteBuffers(static_cast<GLsizei>(VBOs.size()), VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(VAOs.size()), VAOs.data());
    }

    VBOs.resize(newMeshes.size());
    VAOs.resize(newMeshes.size());
    glGenVertexArrays(newMeshes.size(), VAOs.data());
    glGenBuffers(newMeshes.size(), VBOs.data());

    std::vector<GLsizei> vertexCounts(newMeshes.size());

    for (GLsizei i = 0; i < newMeshes.size(); i++) {
        centerAndNormalizeOBJ(newMeshes[i].vertices);

        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);

        glBufferData(GL_ARRAY_BUFFER,
            newMeshes[i].vertices.size() * sizeof(GLfloat),
            newMeshes[i].vertices.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        vertexCounts[i] = newMeshes[i].vertices.size() / 8;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vertexCounts;
}


GLuint Manager3D::reloadTexture(const std::string& path, GLuint oldTexture) {
    if (!path.empty()) {
        if (oldTexture != 0) {
            glDeleteTextures(1, &oldTexture);
        }
        GLuint newTex = loadTexture(path.c_str());
        if (newTex == 0) {
            std::cerr << "reloadTexture: failed to load: " << path << "\n";
            return oldTexture; 
        }
        return newTex;
    }
    return oldTexture;
}




