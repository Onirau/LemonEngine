#pragma once
#include <vector>
#include <string>
#include "Mesh.h"

namespace Engine::Graphics {

class Model {
public:
    // Default constructor for runtime-generated meshes
    Model() = default;

    // Load model from file (OBJ, FBX, GLTF, etc.)
    Model(const std::string& path);

    ~Model() = default;

    void Draw() const;

    std::vector<Mesh>& GetMeshes() { return m_Meshes; }

private:
    std::vector<Mesh> m_Meshes;
    std::string m_Directory;

    void LoadModel(const std::string& path);
    void ProcessNode(/* node data from file */);
    Mesh ProcessMesh(/* mesh data from file */);

    // Optional: store loaded textures to avoid duplicates
    std::vector<Texture2D*> m_LoadedTextures;
};

} // namespace Engine::Graphics
