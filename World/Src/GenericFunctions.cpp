#include "../Includes/GenericFunctions.h"
#include "../Src/ModelLoader/AppCon.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 0.0f, lastY = 0.0f;
    static bool first = true;
    if (first) { lastX = static_cast<float>(xpos); lastY = static_cast<float>(ypos); first = false; }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    AppContext* ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (ctx && ctx->camera) {
        ctx->camera->ProcessMouse(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    AppContext* ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (ctx) {
        ctx->Colors.OnScroll(yoffset);
    }
}

void processInput(GLFWwindow* window, float dt) {
    AppContext* ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->camera) return;
    Camera* cam = ctx->camera;

    bool isMoving = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { cam->MoveForward(dt); isMoving = true; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam->MoveBackward(dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam->MoveLeft(dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam->MoveRight(dt);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

    cam->UpdateFOV(isMoving, dt);
}

void processInputsObject(GLFWwindow* window, glm::vec3& objectPos, glm::vec3& objectRotation, float deltaTime) {
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

void getPlaneData(int& height, int& width) {
    std::cout << "height : ";
    std::cin >> height;
    std::cout << "\nwidth : ";
    std::cin >> width;
}

void GetPathPrompt(HWND window, OPEN_MODES filter) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    switch (filter) {
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
