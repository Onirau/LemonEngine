#pragma once
#include <vector>
#include "../datatypes/Vector3.h"
#include "../core/WindowManager.h"
#include <glm/glm.hpp>

// Forward declarations
struct BasePart;

// External global texture - changed to pointer to match implementation
namespace Engine { namespace Graphics { class Texture2D; } }
extern Engine::Graphics::Texture2D* g_defaultTexture;

// Function declarations - corrected return types
void GenerateDefaultTexture(int width = 128, int height = 128);

// Matrix and lighting functions - use glm::mat4 instead of undefined Matrix
glm::mat4 GetLightSpaceMatrix(const Vector3 &lightDir, const Vector3 &sceneCenter);

void RenderShadowMap(const std::vector<BasePart *> &instances,
                     const Vector3 &lightDir, const Vector3 &sceneCenter);

void PrepareRenderer();
void RenderScene(Camera3D g_camera, std::vector<BasePart *> g_instances);
void UnprepareRenderer();