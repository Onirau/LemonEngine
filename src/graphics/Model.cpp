#include "Model.h"

namespace Engine::Graphics {

Model::Model(const std::string& path)
{
    m_Directory = "";
    // Model loading from file not implemented yet
}

void Model::Draw() const
{
    for (const auto& mesh : m_Meshes)
    {
        mesh.Draw();
    }
}

void Model::LoadModel(const std::string& path)
{
    // Stub - not implemented yet
}

void Model::ProcessNode()
{
    // Stub - not implemented yet
}

Mesh Model::ProcessMesh()
{
    // Return empty mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    return Mesh(vertices, indices, textures);
}

} // namespace Engine::Graphics