//GenericFunctions.cpp
#include "../includes/GenericFunctions.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 800 / 2, lastY = 600 / 2;
    
    static bool first = true;
    if (first) { lastX = xpos; lastY = ypos; first = false; }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;

    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    cam->ProcessMouse(xoffset, yoffset);
}



void processInput(GLFWwindow* window, float dt) {
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    bool isMoving = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { cam->MoveForward(dt); isMoving = true; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {cam->MoveBackward(dt);}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {cam->MoveLeft(dt);}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {cam->MoveRight(dt);}
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwTerminate();

    cam->UpdateFOV(isMoving,dt);
}



void processInputsObject(GLFWwindow* window, glm::vec3& objectPos, glm::vec3& objectRotation, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) objectPos.z -= 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) objectPos.z += 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) objectPos.x -= 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) objectPos.x += 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) objectPos.y += 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) objectPos.y -= 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) objectPos = glm::vec3(0, 0.5f, 0);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) objectRotation.x += 9.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) objectRotation.x -= 9.0f * deltaTime;

}


void dynamicPlaneDraw(float* planeVertices, size_t count,
    unsigned int& planeVAO, unsigned int& planeVBO)
{
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), planeVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}


void getPlaneData(int& height, int& width)
{
    std::cout << "height : ";
    std::cin >> height;
    std::cout << "\nwidth : ";
    std::cin >> width;
}


void generatePlanePoints(float* planeVertices,float width, float height)
{
    float nx = 0.0f, ny = 1.0f, nz = 0.0f;

    std::vector<float> vertices = {
        0,      0,      0,    nx,ny,nz,     0,0,
        width,  0,      0,    nx,ny,nz,     1,0,
        width,  0,  height,   nx,ny,nz,     1,1,
        0,      0,  height,   nx,ny,nz,     0,1
    };

    for (size_t i = 0; i < 32; i++)
    {
        planeVertices[i] = vertices[i];
    }
}



void GetPathPrompt(HWND window,OPEN_MODES filter)
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    switch (filter)
    {
    case AUDIO:
        ofn.lpstrFilter = L"Audio Files *.wav;*.mp3;";
        break;
    case VIDEO:
        ofn.lpstrFilter = L"Video Files *.mp4;*.m4a;";
        break;
    case GENERAL_PURPOSE:
        ofn.lpstrFilter = L"General purposes files *.*;";

        break;
    case OBJECT3D:
        ofn.lpstrFilter = L"Supported 3D files *.obj;";
        break;
    default:
        ofn.lpstrFilter = L"General purposes files *.*;";
        break;
    }
    ofn.hwndOwner = NULL;
    wchar_t fileBuffer[MAX_PATH] = { 0 };
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = MAX_PATH;
    auto temp = GetOpenFileName(&ofn);
    std::cout << temp;
}