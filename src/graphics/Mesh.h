#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../Graphics/Texture2D.h"

namespace Engine::Graphics {

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    Texture2D* texture;
    std::string type; // "diffuse", "specular", etc.
};

class Mesh {
public:
    // Constructor
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         const std::vector<Texture>& textures);

    ~Mesh() = default;

    // Render the mesh using OpenGL
    void Draw() const;

    // Getters
    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    const std::vector<Texture>& GetTextures() const { return m_Textures; }

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;

    // OpenGL buffers
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    void SetupMesh();
};

} // namespace Engine::Graphics
