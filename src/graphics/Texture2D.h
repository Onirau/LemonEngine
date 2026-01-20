#pragma once
#include <string>

namespace Engine::Graphics {

class Texture2D {
public:
    // Constructor loads a texture from file
    Texture2D(const std::string& path);

    // Optional: create empty texture with width/height
    Texture2D(int width, int height, unsigned char* data = nullptr);

    ~Texture2D();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    unsigned int GetID() const { return m_RendererID; }

private:
    unsigned int m_RendererID = 0;
    int m_Width = 0;
    int m_Height = 0;

    void CreateTextureFromFile(const std::string& path);
    void CreateEmptyTexture(int width, int height, unsigned char* data);
};

} // namespace Engine::Graphics
