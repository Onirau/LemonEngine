#include "Model.h"

namespace Engine::Graphics {

Model::Model(const std::string& path)
{
    // Stub: no real model loading
    m_Directory = "";
    // Normally: LoadModel(path);
}

void Model::Draw() const
{
    // Stub: just iterate over meshes
    for (const auto& mesh : m_Meshes)
    {
        mesh.Draw();
    }
}

void Model::LoadModel(const std::string& path)
{
    // Stub, does nothing
}

void Model::ProcessNode()
{
    // Stub
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
