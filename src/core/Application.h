#pragma once

#include <cmath>
#include <filesystem>
#include <vector>

#include <lua.h>
#include <luacode.h>
#include <lualib.h>

#include "../Global.h"
#include "PrimitiveModels.h"
#include "SkyboxRenderer.h"
#include "TextRenderer.h"

class Application {
protected:
    bool rotatingCamera = false;
    bool warpThisFrame = false;
    Vector2 anchorPos = {0, 0};
    Vector3 worldUp = {0, 1, 0};
    float gYaw = 0.0f;
    float gPitch = 0.0f;
    DataModel *dataModel = nullptr;
    Workspace *workspace = nullptr;

    virtual void RenderUI() = 0;
    virtual void Initialize() = 0;
    virtual void PostLuaInitialize() {}
    virtual void Cleanup() = 0;

public:
    Application() {
        dataModel = DataModel::GetInstance();
        workspace = dataModel->WorkspaceService;
    }

    virtual ~Application() {
        delete dataModel;
        UnloadPrimitiveModels();
        g_instances.clear();
        UnloadSkybox();
        TextRenderer::Shutdown();
        WindowManager::CloseWindow();
        lua_close(L_main);
    }

    void Run() {
        Initialize();

        WindowManager::SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        WindowManager::SetConfigFlags(FLAG_VSYNC_HINT);
        WindowManager::InitWindow(1280, 720, GetWindowTitle());
        WindowManager::SetTargetFPS(0);

        TextRenderer::Initialize();

        L_main = luaL_newstate();
        luaL_openlibs(L_main);

        LuaBindings::RegisterScriptBindings(L_main, g_instances, g_camera);

        // Call post-Lua initialization hook
        PostLuaInitialize();

        PrepareRenderer();
        LoadSkybox();

        // Initialize camera
        g_camera = {};
        g_camera.position = Vector3{0, 2, -5};
        g_camera.target = Vector3{0, 2, 0};
        g_camera.up = Vector3{0.0f, 1.0f, 0.0f};
        g_camera.fovy = 70.0f;
        g_camera.projection = CAMERA_PERSPECTIVE;

        workspace->ChildAdded.Connect([](Instance *child) {
            if (auto *part = dynamic_cast<BasePart *>(child)) {
                if (std::find(g_instances.begin(), g_instances.end(), part) ==
                    g_instances.end()) {
                    g_instances.push_back(part);
                }
            }
        });

        workspace->ChildRemoved.Connect([](Instance *child) {
            if (auto *part = dynamic_cast<BasePart *>(child)) {
                auto it =
                    std::find(g_instances.begin(), g_instances.end(), part);
                if (it != g_instances.end()) {
                    g_instances.erase(it);
                }
            }
        });

        MainLoop();
        Cleanup();
    }

protected:
    virtual const char *GetWindowTitle() const = 0;

private:
    void MainLoop() {
        while (!WindowManager::WindowShouldClose()) {
            const double deltaTime = WindowManager::GetFrameTime();
            float moveSpeed = 25.0f * static_cast<float>(deltaTime);

            if (WindowManager::IsKeyDown(KEY_LEFT_SHIFT)) {
                moveSpeed *= 0.25;
            }

            if (WindowManager::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                anchorPos = WindowManager::GetMousePosition();
                rotatingCamera = true;
            }
            if (WindowManager::IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
                rotatingCamera = false;
            }

            if (rotatingCamera) {
                if (!warpThisFrame) {
                    Vector2 currentPos = WindowManager::GetMousePosition();
                    Vector2 delta = {currentPos.x - anchorPos.x, currentPos.y - anchorPos.y};
                    gYaw += delta.x * 0.004f;
                    gPitch += -delta.y * 0.004f;

                    const float limit = 3.14159f / 2 - 0.01f;
                    if (gPitch > limit)
                        gPitch = limit;
                    if (gPitch < -limit)
                        gPitch = -limit;
                } else {
                    WindowManager::SetMousePosition((int)anchorPos.x, (int)anchorPos.y);
                }
                warpThisFrame = !warpThisFrame;
            }

            Vector3 forward = {cosf(gPitch) * cosf(gYaw), sinf(gPitch),
                               cosf(gPitch) * sinf(gYaw)};

            Vector3 right = {
                worldUp.y * forward.z - worldUp.z * forward.y,
                worldUp.z * forward.x - worldUp.x * forward.z,
                worldUp.x * forward.y - worldUp.y * forward.x
            };
            float rightLen = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
            if (rightLen < 1e-6f) {
                right = {1, 0, 0};
            } else {
                right.x /= rightLen;
                right.y /= rightLen;
                right.z /= rightLen;
            }

            Vector3 up = {
                forward.y * right.z - forward.z * right.y,
                forward.z * right.x - forward.x * right.z,
                forward.x * right.y - forward.y * right.x
            };

            Vector3 delta = {0, 0, 0};
            if (WindowManager::IsKeyDown(KEY_W)) {
                delta.x += forward.x * moveSpeed;
                delta.y += forward.y * moveSpeed;
                delta.z += forward.z * moveSpeed;
            }
            if (WindowManager::IsKeyDown(KEY_S)) {
                delta.x -= forward.x * moveSpeed;
                delta.y -= forward.y * moveSpeed;
                delta.z -= forward.z * moveSpeed;
            }
            if (WindowManager::IsKeyDown(KEY_D)) {
                delta.x -= right.x * moveSpeed;
                delta.y -= right.y * moveSpeed;
                delta.z -= right.z * moveSpeed;
            }
            if (WindowManager::IsKeyDown(KEY_A)) {
                delta.x += right.x * moveSpeed;
                delta.y += right.y * moveSpeed;
                delta.z += right.z * moveSpeed;
            }
            if (WindowManager::IsKeyDown(KEY_SPACE) || WindowManager::IsKeyDown(KEY_E)) {
                delta.x += up.x * moveSpeed;
                delta.y += up.y * moveSpeed;
                delta.z += up.z * moveSpeed;
            }
            if (WindowManager::IsKeyDown(KEY_Q)) {
                delta.x -= up.x * moveSpeed;
                delta.y -= up.y * moveSpeed;
                delta.z -= up.z * moveSpeed;
            }

            g_camera.position.x += delta.x;
            g_camera.position.y += delta.y;
            g_camera.position.z += delta.z;
            
            g_camera.target.x = g_camera.position.x + forward.x;
            g_camera.target.y = g_camera.position.y + forward.y;
            g_camera.target.z = g_camera.position.z + forward.z;
            
            g_camera.up = up;

            TaskScheduler_Step();

            WindowManager::BeginDrawing();
            WindowManager::ClearBackground(RAYWHITE);

            RenderScene(g_camera, g_instances);
            RenderUI();

            WindowManager::EndDrawing();
        }
    }
};