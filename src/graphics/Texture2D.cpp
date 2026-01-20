#include "Texture2D.h"
#include <glad/glad.h>   // OpenGL loader
#include "stb_image.h"
#include <iostream>

namespace Engine::Graphics {

    // Constructor: load from file
    Texture2D::Texture2D(const std::string& path) {
        CreateTextureFromFile(path);
    }

    // Constructor: create empty texture (optional data)
    Texture2D::Texture2D(int width, int height, unsigned char* data) {
        CreateEmptyTexture(width, height, data);
    }

    // Destructor: free GPU texture
    Texture2D::~Texture2D() {
        if (m_RendererID != 0) {
            glDeleteTextures(1, &m_RendererID);
        }
    }

    // Bind texture to a texture slot
    void Texture2D::Bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    // Unbind texture
    void Texture2D::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ------------------- Private Methods -------------------

    void Texture2D::CreateTextureFromFile(const std::string& path) {
        int nrChannels;
        stbi_set_flip_vertically_on_load(1); // Flip image vertically
        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load texture: " << path << "\n";
            return;
        }

        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        else format = GL_RGB;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    void Texture2D::CreateEmptyTexture(int width, int height, unsigned char* data) {
        m_Width = width;
        m_Height = height;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        // Use RGBA format by default for empty textures
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Set filtering and wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

} // namespace Engine::Graphics
