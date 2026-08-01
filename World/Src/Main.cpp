//main.cpp
#include <algorithm>
#include "../Includes/GenericFunctions.h"
#include "../Includes/Skybox.h"
#include "ModelLoader/AppCon.h"
#include "../Includes/FrameBuffer.h"
#include "../Includes/BlackHole.h"
#include "../Includes/UIOverlay.h"
#include "../Includes/Utils.h"


Primitives::MeshData CreateCubeMesh(GLfloat size) {
    Primitives::MeshData data;
    GLfloat h = size * 0.5f;

    auto pushVert = [&](glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) {
        data.vertices.insert(data.vertices.end(), {
            pos.x, pos.y, pos.z,
            uv.x, uv.y,
            normal.x, normal.y, normal.z
            });
        };

    auto pushFace = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 normal) {
        GLuint base = static_cast<GLuint>(data.vertices.size() / 8);
        pushVert(a, glm::vec2(0, 0), normal);
        pushVert(b, glm::vec2(1, 0), normal);
        pushVert(c, glm::vec2(1, 1), normal);
        pushVert(d, glm::vec2(0, 1), normal);
        data.indices.insert(data.indices.end(), {
            base + 0, base + 1, base + 2,
            base + 2, base + 3, base + 0
            });
        };

    pushFace({ -h,-h, h }, { h,-h, h }, { h, h, h }, { -h, h, h }, { 0, 0, 1 });
    pushFace({ h,-h,-h }, { -h,-h,-h }, { -h, h,-h }, { h, h,-h }, { 0, 0,-1 });
    pushFace({ -h,-h,-h }, { -h,-h, h }, { -h, h, h }, { -h, h,-h }, { -1, 0, 0 });
    pushFace({ h,-h, h }, { h,-h,-h }, { h, h,-h }, { h, h, h }, { 1, 0, 0 });
    pushFace({ -h, h, h }, { h, h, h }, { h, h,-h }, { -h, h,-h }, { 0, 1, 0 });
    pushFace({ -h,-h,-h }, { h,-h,-h }, { h,-h, h }, { -h,-h, h }, { 0,-1, 0 });

    return data;
}



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1800, 1040, "OpenGLWorld", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader modelShader("Shaders/model.vert", "Shaders/model.frag");
    Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");
    Shader planeShader("Shaders/plane.vert", "Shaders/plane.frag");
    Shader pointShader("Shaders/point.vert", "Shaders/point.frag");
    Shader uiShader("Shaders/UI.vert", "Shaders/UI.frag");
    Shader blackholeShader("Shaders/blackHole.vert", "Shaders/blackHole.frag");
    Shader diskShader("Shaders/horizon.vert", "Shaders/horizon.frag");
    Shader distortShader("Shaders/distort.vert", "Shaders/distort.frag");

    Camera camera(glm::vec3(0, 2, 6));

    AppContext ctx;
    ctx.camera = &camera;
    glfwSetWindowUserPointer(window, &ctx);

    std::vector<std::string> faces = {
        "assets/skybox/right.jpg", "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",  "assets/skybox/down.jpg",
        "assets/skybox/front.jpg","assets/skybox/back.jpg"
    };
    Skybox skybox(faces);

    int planeHeight, planeWidth;
    getPlaneData(planeHeight, planeWidth);
    auto planeData = Primitives::Plane(static_cast<GLfloat>(planeWidth), static_cast<GLfloat>(planeHeight));
    Mesh planeMesh(planeData.vertices, { {0, 3}, {1, 3}, {2, 2} }, GL_DYNAMIC_DRAW);

    BlackHole blackHole(glm::vec3(0.0f, 5.0f, 0.0f));

    int sceneWidth, sceneHeight;
    glfwGetFramebufferSize(window, &sceneWidth, &sceneHeight);
    if (sceneWidth == 0) sceneWidth = 1800;
    if (sceneHeight == 0) sceneHeight = 1040;
    FrameBuffer sceneFB(sceneWidth, sceneHeight);

    auto quadData = Primitives::FullscreenQuad();
    Mesh screenQuad(quadData.vertices, { {0, 2}, {1, 2} });

    UIOverlay ui(static_cast<GLfloat>(sceneWidth) / static_cast<GLfloat>(sceneHeight));

    ctx.modelMeshes = ctx.Mng3D.LoadModel(RootPath("assets/models/Bush.obj"));
    auto cubeData = CreateCubeMesh(1.0f);
    AnimatedObject cube(cubeData.vertices, cubeData.indices, { {0, 3}, {1, 2}, {2, 3} });
    cube.SetBasePosition(glm::vec3(10.0f, 0.0f, 10.0f));

    std::vector<RollingModel> rollingObjects;
    static const std::string kRollingModelPath = "assets/models/Bird.obj";
    glm::vec3 objectPos(0, 0.5f, 0);
    glm::vec3 objectRotation(0, 0, 0);
    GLfloat lastFrame = 0;

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
        GLfloat deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        if (fbHeight == 0) fbHeight = 1;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            static_cast<GLfloat>(fbWidth) / static_cast<GLfloat>(fbHeight), 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        processInput(window, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            static GLfloat lastPress = 0;
            if (currentFrame - lastPress > 0.5f) {
                ctx.modelMeshes = ctx.Mng3D.LoadModel(RootPath("assets/models/Bird.obj"));
                lastPress = currentFrame;
                std::cout << "Model reloaded!\n";
            }   
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            static GLfloat lastClick = 0;
            if (currentFrame - lastClick > 0.2f) {
                glm::vec3 clickPoint = Utils::GetClickPoint(camera.Position, camera.Front, 10.0f);
                ctx.Pbuild.AddPoint(clickPoint, ctx.Colors.CurrentColor());
                lastClick = currentFrame;

                if (ctx.Pbuild.activeChain.size() % 4 == 0) {
                    glm::vec3 centroid(0.0f);
                    for (int idx : ctx.Pbuild.activeChain) centroid += ctx.Pbuild.points[idx];
                    centroid /= 4.0f;
                    centroid.y += 0.5f; 

                    SpawnRollingModel(rollingObjects, ctx.Mng3D, kRollingModelPath, centroid);
                    ctx.Pbuild.ResetChain(); 
                }
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            static GLfloat lastRightClick = 0;
            if (currentFrame - lastRightClick > 0.2f) {
                ctx.Pbuild.ResetChain();
                lastRightClick = currentFrame;
                std::cout << "Chain reset - next point starts a new group\n";
            }
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            static GLfloat lastClear = 0;
            if (currentFrame - lastClear > 0.5f) {
                ctx.Pbuild.Clear();
                lastClear = currentFrame;
                std::cout << "Points cleared!\n";
            }
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            static float lastToggle = 0;
            if (currentFrame - lastToggle > 0.3f) {
                blackHole.Enabled = !blackHole.Enabled;
                lastToggle = currentFrame;
                std::cout << "Black hole " << (blackHole.Enabled ? "enabled" : "disabled") << "\n";
            }
        }

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            static GLfloat lastG = 0;
            if (currentFrame - lastG > 0.4f) {
                if (cube.IsRotating()) {
                    cube.StopRotate();
                    std::cout << "Cube rotation stopped\n";
                }
                else {
                    cube.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);
                    std::cout << "Cube rotation started\n";
                }
                lastG = currentFrame;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            static GLfloat lastH = 0;
            if (currentFrame - lastH > 0.4f) {
                cube.Vibrate(0.15f, 12.0f, 0.5f);
                lastH = currentFrame;
                std::cout << "Cube vibrating\n";
            }
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            static GLfloat lastJ = 0;
            if (currentFrame - lastJ > 0.4f) {
                glm::vec3 current = ctx.AnimObj.GetPose().position;
                cube.Transform(current + glm::vec3(0.0f, 0.0f, -3.0f), 1.0f);
                lastJ = currentFrame;
                std::cout << "Cube moving forward\n";
            }
        }

        ctx.AnimObj.Update(deltaTime);
        cube.Update(deltaTime);

        for (auto& obj : rollingObjects) obj.anim.Update(deltaTime);
        rollingObjects.erase(
            std::remove_if(rollingObjects.begin(), rollingObjects.end(),
                [](const RollingModel& o) { return !o.anim.IsSpinning(); }),
            rollingObjects.end()
        );

        processInputsObject(window, objectPos, objectRotation, deltaTime);

        sceneFB.Bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);
        planeShader.setMat4("model", glm::mat4(1.0f));
        planeShader.setVec3("viewPos", camera.Position);
        planeMesh.Draw(GL_TRIANGLE_FAN);

        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), objectPos);
        model = glm::rotate(model, glm::radians(objectRotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(objectRotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(objectRotation.z), glm::vec3(0, 0, 1));
        modelShader.setMat4("model", model);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setVec3("lightDir", glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)));
        for (auto& mesh : ctx.modelMeshes) mesh.Draw(GL_TRIANGLES);

        modelShader.setMat4("model", cube.GetModelMatrix());
        cube.Draw(GL_TRIANGLES);

        for (auto& obj : rollingObjects) {
            modelShader.setMat4("model", obj.anim.GetPose().GetModelMatrix());
            for (auto& mesh : obj.meshes) mesh.Draw(GL_TRIANGLES);
        }

        blackHole.Draw(blackholeShader, diskShader, view, projection, currentFrame);

        pointShader.use();
        ctx.Pbuild.DrawPoints(pointShader, view, projection);
        ctx.Pbuild.DrawTriangles(pointShader, view, projection);

        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        skybox.Draw(skyboxShader, view, projection);

        FrameBuffer::Unbind();
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Utils::LensingData lensing = Utils::ComputeLensingData(
            blackHole.Position, blackHole.SphereRadius, camera, view, projection);
        bool applyLensing = blackHole.Enabled && lensing.visible;

        glDisable(GL_DEPTH_TEST);

        distortShader.use();
        glUniform1i(glGetUniformLocation(distortShader.ID, "enabled"), applyLensing ? 1 : 0);
        glUniform2f(glGetUniformLocation(distortShader.ID, "blackHoleScreenPos"), lensing.screenUV.x, lensing.screenUV.y);
        glUniform1f(glGetUniformLocation(distortShader.ID, "horizonRadius"), lensing.horizonRadiusUV);
        glUniform1f(glGetUniformLocation(distortShader.ID, "lensStrength"), 3.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneFB.GetColorTexture());
        glUniform1i(glGetUniformLocation(distortShader.ID, "sceneTexture"), 0);
        screenQuad.Draw(GL_TRIANGLES);

        ui.Draw(uiShader, ctx.Colors.CurrentColor());

        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();

        static GLfloat lastTitleUpdate = 0.0f;
        if (currentFrame - lastTitleUpdate > 0.1f) {
            char titleBuf[256];
            snprintf(titleBuf, sizeof(titleBuf),
                "OpenGLWorld - Pos: (%.1f, %.1f, %.1f)  Yaw: %.0f  Pitch: %.0f",
                camera.Position.x, camera.Position.y, camera.Position.z,
                camera.Yaw, camera.Pitch);
            glfwSetWindowTitle(window, titleBuf);
            lastTitleUpdate = currentFrame;
        }
    }

    glfwTerminate();
    return 0;
}