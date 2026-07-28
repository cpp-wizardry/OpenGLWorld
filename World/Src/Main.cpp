//main.cpp
#include "../Includes/GenericFunctions.h"
#include "../Includes/Skybox.h"
#include "ModelLoader/AppCon.h"
#include "../Includes/Primitives.h"
#include "../Includes/FrameBuffer.h"
#include "../Includes/BlackHole.h"
#include "../Includes/UIOverlay.h"
#include "../Includes/Utils.h"

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
                ctx.modelMeshes = ctx.Mng3D.LoadModel(RootPath("assets/models/Bush.obj"));
                lastPress = currentFrame;
                std::cout << "Model reloaded!\n";
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            static GLfloat lastClick = 0;
            if (currentFrame - lastClick > 0.2f) {
                glm::vec3 clickPoint = Utils::GetClickPoint(camera.Position, camera.Front, 10.0f);

                int nearest = ctx.Pbuild.FindNearestPoint(clickPoint, 0.3f);
                if (nearest >= 0) {
                    ctx.Pbuild.SelectPoint(nearest);
                }
                else {
                    ctx.Pbuild.AddPoint(clickPoint, ctx.Colors.CurrentColor());
                    ctx.Pbuild.SelectPoint(static_cast<int>(ctx.Pbuild.points.size()) - 1);
                }
                lastClick = currentFrame;
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
