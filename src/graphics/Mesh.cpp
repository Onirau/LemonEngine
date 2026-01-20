#include "Mesh.h"
#include <glad/glad.h>

namespace Engine::Graphics {

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const std::vector<Texture>& textures)
    : m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
    // Minimal setup to satisfy compilation
    VAO = VBO = EBO = 0;
    // Normally: SetupMesh(); but left empty for now
}

void Mesh::Draw() const
{
    // Empty stub to satisfy compilation
    // Normally: bind VAO, bind textures, glDrawElements
}

void Mesh::SetupMesh()
{
    // Stub, does nothing for now
}

} // namespace Engine::Graphics
