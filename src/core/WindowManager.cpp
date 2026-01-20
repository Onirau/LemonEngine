#include "WindowManager.h"
#include <iostream>
#include <chrono>
#include <unordered_map>

GLFWwindow* WindowManager::window = nullptr;
int WindowManager::windowWidth = 800;
int WindowManager::windowHeight = 600;
double WindowManager::lastFrameTime = 0.0;
double WindowManager::deltaTime = 0.0;
bool WindowManager::vsyncEnabled = false;

// Track key states for pressed/released detection
static std::unordered_map<int, bool> previousKeyStates;
static std::unordered_map<int, bool> previousMouseStates;

void WindowManager::errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void WindowManager::framebufferSizeCallback(GLFWwindow* win, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void WindowManager::InitWindow(int width, int height, const char* title) {
    glfwSetErrorCallback(errorCallback);
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    lastFrameTime = glfwGetTime();
}

void WindowManager::CloseWindow() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool WindowManager::WindowShouldClose() {
    return window ? glfwWindowShouldClose(window) : true;
}

void WindowManager::SetConfigFlags(unsigned int flags) {
    if (flags & FLAG_WINDOW_RESIZABLE) {
        if (window) {
            glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);
        } else {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }
    }
    
    if (flags & FLAG_VSYNC_HINT) {
        vsyncEnabled = true;
    }
}

void WindowManager::SetTargetFPS(int fps) {
    if (fps == 0) {
        glfwSwapInterval(vsyncEnabled ? 1 : 0);
    } else {
        glfwSwapInterval(0);
    }
}

int WindowManager::GetScreenWidth() {
    return windowWidth;
}

int WindowManager::GetScreenHeight() {
    return windowHeight;
}

void WindowManager::UpdateDeltaTime() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
}

double WindowManager::GetFrameTime() {
    return deltaTime;
}

int WindowManager::GetFPS() {
    return deltaTime > 0.0 ? static_cast<int>(1.0 / deltaTime) : 0;
}

bool WindowManager::IsKeyDown(int key) {
    return window ? glfwGetKey(window, key) == GLFW_PRESS : false;
}

bool WindowManager::IsKeyPressed(int key) {
    if (!window) return false;
    
    bool currentState = glfwGetKey(window, key) == GLFW_PRESS;
    bool wasPressed = currentState && !previousKeyStates[key];
    previousKeyStates[key] = currentState;
    return wasPressed;
}

bool WindowManager::IsKeyReleased(int key) {
    if (!window) return false;
    
    bool currentState = glfwGetKey(window, key) == GLFW_PRESS;
    bool wasReleased = !currentState && previousKeyStates[key];
    previousKeyStates[key] = currentState;
    return wasReleased;
}

bool WindowManager::IsMouseButtonPressed(int button) {
    if (!window) return false;
    
    bool currentState = glfwGetMouseButton(window, button) == GLFW_PRESS;
    bool wasPressed = currentState && !previousMouseStates[button];
    previousMouseStates[button] = currentState;
    return wasPressed;
}

bool WindowManager::IsMouseButtonReleased(int button) {
    if (!window) return false;
    
    bool currentState = glfwGetMouseButton(window, button) == GLFW_PRESS;
    bool wasReleased = !currentState && previousMouseStates[button];
    previousMouseStates[button] = currentState;
    return wasReleased;
}

bool WindowManager::IsMouseButtonDown(int button) {
    return window ? glfwGetMouseButton(window, button) == GLFW_PRESS : false;
}

Vector2 WindowManager::GetMousePosition() {
    if (!window) return Vector2(0, 0);
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return Vector2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void WindowManager::SetMousePosition(int x, int y) {
    if (window) {
        glfwSetCursorPos(window, static_cast<double>(x), static_cast<double>(y));
    }
}

void WindowManager::BeginDrawing() {
    if (window) {
        glfwPollEvents();
        UpdateDeltaTime();
    }
}

void WindowManager::EndDrawing() {
    if (window) {
        glfwSwapBuffers(window);
    }
}

void WindowManager::ClearBackground(Color color) {
    glClearColor(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}