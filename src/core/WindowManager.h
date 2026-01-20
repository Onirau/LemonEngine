#pragma once

#include "../datatypes/Color3.h"
#include "../datatypes/Vector3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

// Standard colors
inline Color RAYWHITE{245, 245, 245, 255};
inline Color LIGHTGRAY{200, 200, 200, 255};
inline Color GREEN{0, 228, 48, 255};
inline Color WHITE{255, 255, 255, 255};
inline Color BLANK{0, 0, 0, 0};

// Camera modes
enum CameraProjection {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC
};

struct Camera3D {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float fovy;
    CameraProjection projection;
};

// Key codes (GLFW mapping)
enum KeyboardKey {
    KEY_W = GLFW_KEY_W,
    KEY_S = GLFW_KEY_S,
    KEY_A = GLFW_KEY_A,
    KEY_D = GLFW_KEY_D,
    KEY_Q = GLFW_KEY_Q,
    KEY_E = GLFW_KEY_E,
    KEY_SPACE = GLFW_KEY_SPACE,
    KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT
};

enum MouseButton {
    MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE
};

class WindowManager {
private:
    static GLFWwindow* window;
    static int windowWidth;
    static int windowHeight;
    static double lastFrameTime;
    static double deltaTime;
    static bool vsyncEnabled;

    static void framebufferSizeCallback(GLFWwindow* win, int width, int height);
    static void errorCallback(int error, const char* description);

public:
    // Window management
    static void InitWindow(int width, int height, const char* title);
    static void CloseWindow();
    static bool WindowShouldClose();
    static void SetConfigFlags(unsigned int flags);
    static void SetTargetFPS(int fps);

    // Screen management
    static int GetScreenWidth();
    static int GetScreenHeight();

    // Timing
    static void UpdateDeltaTime();
    static double GetFrameTime();
    static int GetFPS();

    // Input - Keyboard
    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static bool IsKeyReleased(int key);

    // Input - Mouse
    static bool IsMouseButtonPressed(int button);
    static bool IsMouseButtonReleased(int button);
    static bool IsMouseButtonDown(int button);
    static Vector2 GetMousePosition();
    static void SetMousePosition(int x, int y);

    // Drawing
    static void BeginDrawing();
    static void EndDrawing();
    static void ClearBackground(Color color);

    // Get internal window handle
    static GLFWwindow* GetWindow() { return window; }
};

// Config flags
#define FLAG_WINDOW_RESIZABLE 0x0001
#define FLAG_VSYNC_HINT 0x0002