#include "PrimitiveModels.h"
#include <unordered_map>

extern Engine::Graphics::Texture2D* g_defaultTexture;

static std::unordered_map<PartType, Engine::Graphics::Model> g_models;

using namespace Engine::Graphics;

static void CalculateFaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, glm::vec3& normal)
{
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    normal = glm::normalize(glm::cross(edge1, edge2));
}

static Mesh GenMeshCube(float width, float height, float depth)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float w = width * 0.5f;
    float h = height * 0.5f;
    float d = depth * 0.5f;

    // 8 corners of cube
    glm::vec3 cubeVerts[] = {
        {-w,-h,-d}, {w,-h,-d}, {w,h,-d}, {-w,h,-d},   // back face
        {-w,-h,d},  {w,-h,d},  {w,h,d},  {-w,h,d}     // front face
    };

    glm::vec2 uvs[] = {
        {0,0},{1,0},{1,1},{0,1}
    };

    // Define 12 triangles (2 per face)
    int faceIndices[][6] = {
        {0,1,2,2,3,0}, // back
        {4,5,6,6,7,4}, // front
        {0,4,7,7,3,0}, // left
        {1,5,6,6,2,1}, // right
        {3,2,6,6,7,3}, // top
        {0,1,5,5,4,0}  // bottom
    };

    for (auto &face : faceIndices)
    {
        glm::vec3 normal = glm::normalize(glm::cross(
            cubeVerts[face[1]] - cubeVerts[face[0]],
            cubeVerts[face[2]] - cubeVerts[face[0]]
        ));

        for (int i = 0; i < 6; ++i)
        {
            Vertex v;
            v.Position = cubeVerts[face[i]];
            v.Normal = normal;
            v.TexCoords = uvs[i%4];
            v.Tangent = glm::vec3(0.0f);
            v.Bitangent = glm::vec3(0.0f);
            vertices.push_back(v);
            indices.push_back(static_cast<unsigned int>(vertices.size()-1));
        }
    }

    std::vector<Texture> textures;
    Texture tex;
    tex.texture = g_defaultTexture;
    tex.type = "diffuse";
    textures.push_back(tex);

    return Mesh(vertices, indices, textures);
}

static Mesh GenMeshCylinder(float radius, float height, int slices)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float halfHeight = height * 0.5f;

    // Side surface
    for (int i = 0; i < slices; ++i)
    {
        float theta0 = (float)i / slices * 2.0f * Constants::PI_F;
        float theta1 = (float)(i+1) / slices * 2.0f * Constants::PI_F;

        glm::vec3 p0(radius * cos(theta0), -halfHeight, radius * sin(theta0));
        glm::vec3 p1(radius * cos(theta1), -halfHeight, radius * sin(theta1));
        glm::vec3 p2(radius * cos(theta1), halfHeight, radius * sin(theta1));
        glm::vec3 p3(radius * cos(theta0), halfHeight, radius * sin(theta0));

        glm::vec3 normal0 = glm::normalize(glm::vec3(p0.x,0,p0.z));
        glm::vec3 normal1 = glm::normalize(glm::vec3(p1.x,0,p1.z));

        // Two triangles per quad
        vertices.push_back({p0, normal0, {0,0}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p1, normal1, {1,0}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p2, normal1, {1,1}, glm::vec3(0.0f), glm::vec3(0.0f)});

        vertices.push_back({p2, normal1, {1,1}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p3, normal0, {0,1}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p0, normal0, {0,0}, glm::vec3(0.0f), glm::vec3(0.0f)});

        for(int j=0;j<6;j++)
            indices.push_back(static_cast<unsigned int>(vertices.size()-6+j));
    }

    // Top & bottom caps
    glm::vec3 topCenter(0, halfHeight, 0);
    glm::vec3 bottomCenter(0, -halfHeight, 0);

    for (int i = 0; i < slices; ++i)
    {
        float theta0 = (float)i / slices * 2.0f * Constants::PI_F;
        float theta1 = (float)(i+1) / slices * 2.0f * Constants::PI_F;

        glm::vec3 p0(radius * cos(theta0), halfHeight, radius * sin(theta0));
        glm::vec3 p1(radius * cos(theta1), halfHeight, radius * sin(theta1));
        glm::vec3 p2(radius * cos(theta0), -halfHeight, radius * sin(theta0));
        glm::vec3 p3(radius * cos(theta1), -halfHeight, radius * sin(theta1));

        glm::vec3 topNormal(0,1,0);
        glm::vec3 bottomNormal(0,-1,0);

        // Top cap
        vertices.push_back({topCenter, topNormal, {0.5f,0.5f}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p0, topNormal, {0,0}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p1, topNormal, {1,0}, glm::vec3(0.0f), glm::vec3(0.0f)});
        indices.push_back(static_cast<unsigned int>(vertices.size()-3));
        indices.push_back(static_cast<unsigned int>(vertices.size()-2));
        indices.push_back(static_cast<unsigned int>(vertices.size()-1));

        // Bottom cap
        vertices.push_back({bottomCenter, bottomNormal, {0.5f,0.5f}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p3, bottomNormal, {1,0}, glm::vec3(0.0f), glm::vec3(0.0f)});
        vertices.push_back({p2, bottomNormal, {0,0}, glm::vec3(0.0f), glm::vec3(0.0f)});
        indices.push_back(static_cast<unsigned int>(vertices.size()-3));
        indices.push_back(static_cast<unsigned int>(vertices.size()-2));
        indices.push_back(static_cast<unsigned int>(vertices.size()-1));
    }

    std::vector<Texture> textures;
    Texture tex;
    tex.texture = g_defaultTexture;
    tex.type = "diffuse";
    textures.push_back(tex);

    return Mesh(vertices, indices, textures);
}

static Mesh GenMeshSphere(float radius, int slices, int stacks)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(int i=0;i<=stacks;i++)
    {
        float phi = i * Constants::PI_F / stacks;
        for(int j=0;j<=slices;j++)
        {
            float theta = j * 2.0f * Constants::PI_F / slices;

            glm::vec3 pos(
                radius * sin(phi)*cos(theta),
                radius * cos(phi),
                radius * sin(phi)*sin(theta)
            );

            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 uv(theta/(2*Constants::PI_F), phi/Constants::PI_F);

            vertices.push_back({pos, normal, uv, glm::vec3(0.0f), glm::vec3(0.0f)});
        }
    }

    for(int i=0;i<stacks;i++)
    {
        for(int j=0;j<slices;j++)
        {
            unsigned int first = i*(slices+1) + j;
            unsigned int second = first + slices +1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first+1);

            indices.push_back(second);
            indices.push_back(second+1);
            indices.push_back(first+1);
        }
    }

    std::vector<Texture> textures;
    Texture tex;
    tex.texture = g_defaultTexture;
    tex.type = "diffuse";
    textures.push_back(tex);

    return Mesh(vertices, indices, textures);
}


static Mesh GenMeshWedge()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Bottom face (2 triangles)
    glm::vec3 verts[] = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f},
        // Back face
        {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
        // Left triangle
        {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, -0.5f},
        // Right triangle
        {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, 0.5f},
        // Top face
        {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
    };

    glm::vec2 uvs[] = {
        {0,0},{1,0},{0,1},{1,0},{1,1},{0,1},
        {0,0},{0,1},{1,1},{1,0},{0,0},{1,1},
        {0,0},{1,0},{0,1},
        {0,0},{1,0},{0,1},
        {0,0},{0,1},{1,0},{1,0},{0,1},{1,1}
    };

    // Generate vertices with normals
    for (size_t i = 0; i < sizeof(verts)/sizeof(verts[0]); i += 3)
    {
        glm::vec3 normal;
        CalculateFaceNormal(verts[i], verts[i+1], verts[i+2], normal);

        for (int j = 0; j < 3; ++j)
        {
            Vertex v;
            v.Position = verts[i+j];
            v.Normal = normal;
            v.TexCoords = uvs[i+j];
            v.Tangent = glm::vec3(0.0f);
            v.Bitangent = glm::vec3(0.0f);
            vertices.push_back(v);
            indices.push_back(static_cast<unsigned int>(vertices.size()-1));
        }
    }

    std::vector<Texture> textures;
    Texture tex;
    tex.texture = g_defaultTexture;
    tex.type = "diffuse";
    textures.push_back(tex);

    return Mesh(vertices, indices, textures);
}

static Mesh GenMeshCornerWedge()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Vertex positions
    glm::vec3 verts[] = {
        // Bottom face (2 triangles)
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f},

        // Right triangle
        {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},

        // Front triangle
        {-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},

        // Back triangle
        {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f},

        // Left triangle
        {0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
    };

    // Texture coordinates
    glm::vec2 uvs[] = {
        {0,0},{1,0},{0,1},{1,0},{1,1},{0,1},    // Bottom
        {0,0},{1,0},{0,1},                        // Right
        {0,0},{0,1},{1,1},                        // Front
        {0,0},{1,0},{0.5,1},                      // Back
        {1,0},{0,1},{1,1}                         // Left
    };

    // Generate vertices with normals
    for (size_t i = 0; i < sizeof(verts)/sizeof(verts[0]); i += 3)
    {
        glm::vec3 normal;
        CalculateFaceNormal(verts[i], verts[i+1], verts[i+2], normal);

        for (int j = 0; j < 3; ++j)
        {
            Vertex v;
            v.Position = verts[i+j];
            v.Normal = normal;
            v.TexCoords = uvs[i+j];
            v.Tangent = glm::vec3(0.0f);
            v.Bitangent = glm::vec3(0.0f);
            vertices.push_back(v);
            indices.push_back(static_cast<unsigned int>(vertices.size()-1));
        }
    }

    // Assign default diffuse texture
    std::vector<Texture> textures;
    Texture tex;
    tex.texture = g_defaultTexture;
    tex.type = "diffuse";
    textures.push_back(tex);

    return Mesh(vertices, indices, textures);
}

Model LoadModelFromMesh(const Mesh& mesh)
{
    Model model;
    model.GetMeshes().push_back(mesh);
    return model;
}

void PreparePrimitiveModels()
{
    Model block = LoadModelFromMesh(GenMeshCube(1.f,1.f,1.f));
    Model cylinder = LoadModelFromMesh(GenMeshCylinder(0.5f,1.f,16));
    Model ball = LoadModelFromMesh(GenMeshSphere(0.5f,16,16));
    Model wedge = LoadModelFromMesh(GenMeshWedge());
    // Model cornerWedge = LoadModelFromMesh(GenMeshCornerWedge());

    g_models[PartType::Block] = block;
    g_models[PartType::Cylinder] = cylinder;
    g_models[PartType::Ball] = ball;
    g_models[PartType::Wedge] = wedge;
    // g_models[PartType::CornerWedge] = cornerWedge;
}

Model* GetPrimitiveModel(PartType shape)
{
    auto it = g_models.find(shape);
    if (it != g_models.end())
        return &it->second;
    return nullptr;
}

void UnloadPrimitiveModels()
{
    g_models.clear();
}