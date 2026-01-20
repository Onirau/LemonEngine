#pragma once

#include "WindowManager.h"
#include <glad/glad.h>
#include <string>
#include <unordered_map>

struct Character {
    unsigned int textureID;
    int sizeX, sizeY;
    int bearingX, bearingY;
    unsigned int advance;
};

class TextRenderer {
private:
    static std::unordered_map<char, Character> characters;
    static unsigned int VAO, VBO;
    static unsigned int shaderProgram;
    static bool initialized;

    static void InitializeRenderer();
    static void LoadDefaultFont();
    static unsigned int CompileShader(const char* vertexSource, const char* fragmentSource);

public:
    static void Initialize();
    static void Shutdown();
    static void DrawText(const char* text, int x, int y, int fontSize, Color color);
    static const char* TextFormat(const char* format, ...);
};