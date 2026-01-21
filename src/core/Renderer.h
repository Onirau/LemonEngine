#pragma once
#include <vector>
#include "../datatypes/Vector3.h"
#include "../core/WindowManager.h"

// Forward declarations
struct BasePart;

// External global texture
namespace Engine { namespace Graphics { class Texture2D; } }
extern Engine::Graphics::Texture2D g_defaultTexture;

// Function declarations
Engine::Graphics::Texture2D GenerateDefaultTexture(int width = 128, int height = 128);

// Matrix and lighting functions
struct Matrix; // Forward declare Matrix type
Matrix GetLightSpaceMatrix(const Vector3 &lightDir, const Vector3 &sceneCenter);

void RenderShadowMap(const std::vector<BasePart *> &instances,
                     const Vector3 &lightDir, const Vector3 &sceneCenter);

void PrepareRenderer();
void RenderScene(Camera3D g_camera, std::vector<BasePart *> g_instances);
void UnprepareRenderer();