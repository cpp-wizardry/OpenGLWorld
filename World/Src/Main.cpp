//main.cpp
#include "../Includes/GenericFunctions.h"
#include "../Includes/Skybox.h"
#include "../Includes/PointBuilder.h"

static float g_hue = 0.0f;
static glm::vec3 g_currentColor = HueToRGB(0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g_hue += static_cast<float>(yoffset) * 15.0f; 
    if (g_hue < 0.0f) g_hue += 360.0f;
    g_hue = std::fmod(g_hue, 360.0f);
    g_currentColor = HueToRGB(g_hue);
}

std::vector<float> GenerateSphere(float radius, int sectors, int stacks, std::vector<unsigned int>& outIndices) {
    std::vector<float> vertices;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() / 2.0f - i * (glm::pi<float>() / stacks); 
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * (2.0f * glm::pi<float>() / sectors);
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(z);
            vertices.push_back(y);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                outIndices.push_back(k1);
                outIndices.push_back(k2);
                outIndices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                outIndices.push_back(k1 + 1);
                outIndices.push_back(k2);
                outIndices.push_back(k2 + 1);
            }
        }
    }

    return vertices;
}

std::vector<float> GenerateRing(float innerRadius, float outerRadius, int segments) {
    std::vector<float> vertices;

    for (int i = 0; i < segments; ++i) {
        float a0 = i * (2.0f * glm::pi<float>() / segments);
        float a1 = (i + 1) * (2.0f * glm::pi<float>() / segments);

        glm::vec2 innerA(cosf(a0) * innerRadius, sinf(a0) * innerRadius);
        glm::vec2 innerB(cosf(a1) * innerRadius, sinf(a1) * innerRadius);
        glm::vec2 outerA(cosf(a0) * outerRadius, sinf(a0) * outerRadius);
        glm::vec2 outerB(cosf(a1) * outerRadius, sinf(a1) * outerRadius);

        auto push = [&](glm::vec2 p, float dist) {
            vertices.push_back(p.x);
            vertices.push_back(0.0f);
            vertices.push_back(p.y);
            vertices.push_back(dist);
            };

        push(innerA, 0.0f); push(outerA, 1.0f); push(outerB, 1.0f);
        push(innerA, 0.0f); push(outerB, 1.0f); push(innerB, 0.0f);
    }

    return vertices;
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
    glfwSetWindowUserPointer(window, &camera);

    std::vector<std::string> faces = {
        "assets/skybox/right.jpg", "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",  "assets/skybox/down.jpg",
        "assets/skybox/front.jpg","assets/skybox/back.jpg"
    };

    int height, width;
    unsigned int planeVAO = 0;
    unsigned int planeVBO = 0;

    getPlaneData(height, width);

    float planeVertices[32];

    generatePlanePoints(planeVertices, width, height);

    dynamicPlaneDraw(planeVertices, 32, planeVAO, planeVBO);

    Skybox skybox(faces);

    std::vector<unsigned int> sphereIndices;
    std::vector<float> sphereVertices = GenerateSphere(1.5f, 36, 18, sphereIndices);

    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::vector<float> diskVertices = GenerateRing(2.2f, 4.5f, 64);

    unsigned int diskVAO, diskVBO;
    glGenVertexArrays(1, &diskVAO);
    glGenBuffers(1, &diskVBO);
    glBindVertexArray(diskVAO);
    glBindBuffer(GL_ARRAY_BUFFER, diskVBO);
    glBufferData(GL_ARRAY_BUFFER, diskVertices.size() * sizeof(float), diskVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glm::vec3 blackHolePos(0.0f, 5.0f, 0.0f); 

    int sceneWidth, sceneHeight;
    glfwGetFramebufferSize(window, &sceneWidth, &sceneHeight);
    if (sceneWidth == 0) sceneWidth = 1800;
    if (sceneHeight == 0) sceneHeight = 1040;

    unsigned int sceneFBO;
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    unsigned int sceneColorTex;
    glGenTextures(1, &sceneColorTex);
    glBindTexture(GL_TEXTURE_2D, sceneColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTex, 0);

    unsigned int sceneDepthRBO;
    glGenRenderbuffers(1, &sceneDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, sceneWidth, sceneHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sceneDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Scene framebuffer is not complete!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float fullscreenQuadVertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenQuadVertices), fullscreenQuadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);


    float uiSwatchVertices[] = {
        -0.95f,  0.75f,
        -0.75f,  0.75f,
        -0.75f,  0.95f,

        -0.95f,  0.75f,
        -0.75f,  0.95f,
        -0.95f,  0.95f
    };
    unsigned int uiVAO, uiVBO;
    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);
    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiSwatchVertices), uiSwatchVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    float chAspect = 1800.0f / 1040.0f; 
    float chThickness = 0.005f;   
    float chLength = 0.05f;      

    float vBarHalfW = chThickness / chAspect; 
    float hBarHalfH = chThickness;             

    float crosshairVertices[] = {
        -vBarHalfW, -chLength,
         vBarHalfW, -chLength,
         vBarHalfW,  chLength,

        -vBarHalfW, -chLength,
         vBarHalfW,  chLength,
        -vBarHalfW,  chLength,

        -chLength, -hBarHalfH,
         chLength, -hBarHalfH,
         chLength,  hBarHalfH,

        -chLength, -hBarHalfH,
         chLength,  hBarHalfH,
        -chLength,  hBarHalfH
    };
    unsigned int crosshairVAO, crosshairVBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    int w, h, c;
    for (int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &c, 0);
        if (data) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    AppContext ctx;
    ctx.vertexCounts = ctx.Mng3D.reloadModel(RootPath("assets/models/Bush.obj"), ctx.VBOs, ctx.VAOs);

    glm::vec3 objectPos(0, 0.5f, 0);
    glm::vec3 objectRotation(0, 0, 0);
    float lastFrame = 0;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        if (fbHeight == 0) fbHeight = 1;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            static_cast<float>(fbWidth) / static_cast<float>(fbHeight), 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        processInput(window, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            static float lastPress = 0;
            if (currentFrame - lastPress > 0.5f) {
                ctx.vertexCounts = ctx.Mng3D.reloadModel(RootPath("assets/models/Bush.obj"), ctx.VBOs, ctx.VAOs);
                lastPress = currentFrame;
                std::cout << "Model reloaded!\n";
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            static float lastClick = 0;
            if (currentFrame - lastClick > 0.2f) {

                glm::vec3 rayDir = camera.Front;
                glm::vec3 clickPoint = GetClickPoint(camera.Position, rayDir, 10.0f);

                int nearest = ctx.Pbuild.FindNearestPoint(clickPoint, 0.3f);
                if (nearest >= 0) {
                    ctx.Pbuild.SelectPoint(nearest);
                }
                else {
                    ctx.Pbuild.AddPoint(clickPoint, g_currentColor);
                    int newIndex = static_cast<int>(ctx.Pbuild.points.size()) - 1;
                    ctx.Pbuild.SelectPoint(newIndex); 
                }

                lastClick = currentFrame;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            static float lastClear = 0;
            if (currentFrame - lastClear > 0.5f) {
                ctx.Pbuild.Clear();
                lastClear = currentFrame;
                std::cout << "Points cleared!\n";
            }
        }

        processInputsObject(window, objectPos, objectRotation, deltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
        glViewport(0, 0, sceneWidth, sceneHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);
        planeShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), objectPos);

        model = glm::rotate(model, glm::radians(objectRotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(objectRotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(objectRotation.z), glm::vec3(0, 0, 1));

        model = glm::scale(model, glm::vec3(1.0f));
        modelShader.setMat4("model", model);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setVec3("lightDir", glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)));

        for (size_t i = 0; i < ctx.VAOs.size(); ++i) {
            glBindVertexArray(ctx.VAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, ctx.vertexCounts[i] * 8 / 8);
        }

        blackholeShader.use();
        blackholeShader.setMat4("view", view);
        blackholeShader.setMat4("projection", projection);
        glm::mat4 bhModel = glm::translate(glm::mat4(1.0f), blackHolePos);
        blackholeShader.setMat4("model", bhModel);
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        diskShader.use();
        diskShader.setMat4("view", view);
        diskShader.setMat4("projection", projection);
        glm::mat4 diskModel = glm::translate(glm::mat4(1.0f), blackHolePos);
        diskModel = glm::rotate(diskModel, currentFrame * 0.4f, glm::vec3(0, 1, 0));
        diskShader.setMat4("model", diskModel);
        glBindVertexArray(diskVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(diskVertices.size() / 4));
        glBindVertexArray(0);


        pointShader.use();
        ctx.Pbuild.DrawPoints(pointShader, view, projection);
        ctx.Pbuild.DrawTriangles(pointShader, view, projection);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skybox.Draw(skyboxShader, view, projection);

        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec4 clipPos = projection * view * glm::vec4(blackHolePos, 1.0f);
        glm::vec2 bhScreenUV(0.5f, 0.5f);
        if (clipPos.w > 0.0f) {
            glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
            bhScreenUV = glm::vec2(ndc.x, ndc.y) * 0.5f + 0.5f;
        }
        glm::vec3 edgeWorldPos = blackHolePos + camera.Right * 1.5f;
        glm::vec4 edgeClip = projection * view * glm::vec4(edgeWorldPos, 1.0f);
        float horizonRadiusUV = 0.05f;
        if (edgeClip.w > 0.0f && clipPos.w > 0.0f) {
            glm::vec3 edgeNdc = glm::vec3(edgeClip) / edgeClip.w;
            glm::vec2 edgeUV = glm::vec2(edgeNdc.x, edgeNdc.y) * 0.5f + 0.5f;
            horizonRadiusUV = glm::length(edgeUV - bhScreenUV);
        }

        glDisable(GL_DEPTH_TEST);
        distortShader.use();
        glUniform2f(glGetUniformLocation(distortShader.ID, "blackHoleScreenPos"), bhScreenUV.x, bhScreenUV.y);
        glUniform1f(glGetUniformLocation(distortShader.ID, "horizonRadius"), horizonRadiusUV);
        glUniform1f(glGetUniformLocation(distortShader.ID, "lensStrength"), 3.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColorTex);
        glUniform1i(glGetUniformLocation(distortShader.ID, "sceneTexture"), 0);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

        glDisable(GL_DEPTH_TEST);
        uiShader.use();
        uiShader.setVec3("color", g_currentColor);
        glBindVertexArray(uiVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        uiShader.setVec3("color", g_currentColor); 
        glBindVertexArray(crosshairVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();

        static float lastTitleUpdate = 0.0f;
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