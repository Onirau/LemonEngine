#pragma once

struct Skybox {
    Model model{};
    Shader shader{};
};

extern Skybox g_skybox;

void LoadSkybox();
void UnloadSkybox();
void DrawSkybox();