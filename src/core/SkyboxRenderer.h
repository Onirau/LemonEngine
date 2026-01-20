#pragma once

#include "../graphics/Shader.h"
#include <optional>

struct Skybox {
    // Model model{};
    std::optional<Engine::Graphics::Shader> shader;
};

extern Skybox g_skybox;

void LoadSkybox();
void UnloadSkybox();
void DrawSkybox();