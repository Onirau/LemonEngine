#include "Renderer.h"
#include "src/graphics/Shader.h"
#include "src/graphics/Texture2D.h"
#include "src/graphics/Model.h"
#include "PrimitiveModels.h"
#include "SkyboxRenderer.h"
#include "../instances/BasePart.h"
#include "../enums/PartType.h"
#include "src/instances/Part.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>

// Stub implementations for image generation
typedef struct Image {
    void* data;
    int width;
    int height;
    int mipmaps;
    int format;
} Image;

Image GenImageColor(int width, int height, Color color) {
    Image img = {nullptr, width, height, 1, 0};
    img.data = malloc(width * height * 4);
    
    unsigned char* pixels = (unsigned char*)img.data;
    for (int i = 0; i < width * height; i++) {
        pixels[i*4 + 0] = color.r;
        pixels[i*4 + 1] = color.g;
        pixels[i*4 + 2] = color.b;
        pixels[i*4 + 3] = color.a;
    }
    
    return img;
}

void ImageDrawPixel(Image* image, int x, int y, Color color) {
    if (!image || !image->data) return;
    if (x < 0 || x >= image->width || y < 0 || y >= image->height) return;
    
    unsigned char* pixels = (unsigned char*)image->data;
    int index = (y * image->width + x) * 4;
    pixels[index + 0] = color.r;
    pixels[index + 1] = color.g;
    pixels[index + 2] = color.b;
    pixels[index + 3] = color.a;
}

Engine::Graphics::Texture2D* LoadTextureFromImage(Image img) {
    return new Engine::Graphics::Texture2D(img.width, img.height, (unsigned char*)img.data);
}

void UnloadImage(Image img) {
    if (img.data) free(img.data);
}

Engine::Graphics::Texture2D* g_defaultTexture = nullptr;
Engine::Graphics::Shader* g_basicShader = nullptr;

static Color Color3ToColor(const Color3 &c) {
    return Color{(unsigned char)roundf(c.r * 255.0f),
                 (unsigned char)roundf(c.g * 255.0f),
                 (unsigned char)roundf(c.b * 255.0f), 255};
}

void GenerateDefaultTexture(int width, int height) {
    Image img = GenImageColor(width, height, BLANK);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char n = (unsigned char)(rand() % 16 + 239);
            Color c = {n, n, n, 255};
            ImageDrawPixel(&img, x, y, c);
        }
    }

    if (g_defaultTexture) {
        delete g_defaultTexture;
    }
    g_defaultTexture = LoadTextureFromImage(img);
    UnloadImage(img);
}

// Basic vertex shader
static const char* basicVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Basic fragment shader
static const char* basicFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse;
uniform vec3 objectColor;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform float transparency;

void main()
{
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * objectColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * objectColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    vec4 texColor = texture(texture_diffuse, TexCoord);
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    
    FragColor = vec4(result, 1.0 - transparency);
}
)";

PartType StringToPartType(const std::string& shape) {
    if (shape == "Ball" || shape == "Sphere") return PartType::Ball;
    if (shape == "Block") return PartType::Block;
    if (shape == "Cylinder") return PartType::Cylinder;
    if (shape == "Wedge") return PartType::Wedge;
    if (shape == "CornerWedge") return PartType::CornerWedge;
    return PartType::Block;
}

void RenderScene(Camera3D camera, const std::vector<BasePart *> instances) {
    if (!g_basicShader) return;

    g_basicShader->Bind();

    // Set up view and projection matrices
    glm::vec3 cameraPos(camera.position.x, camera.position.y, camera.position.z);
    glm::vec3 cameraTarget(camera.target.x, camera.target.y, camera.target.z);
    glm::vec3 cameraUp(camera.up.x, camera.up.y, camera.up.z);
    
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.fovy),
        (float)WindowManager::GetScreenWidth() / (float)WindowManager::GetScreenHeight(),
        0.1f, 1000.0f
    );

    g_basicShader->SetMat4("view", view);
    g_basicShader->SetMat4("projection", projection);
    g_basicShader->SetVec3("viewPos", cameraPos);
    g_basicShader->SetVec3("lightDir", glm::vec3(0.5f, -1.0f, 0.3f));

    // Render each part
    for (BasePart* part : instances) {
        if (!part) continue;

        // Get the model for this part type
        Engine::Graphics::Model* model = nullptr;
        
        // Try to get Shape from Part (if it's a Part)
        if (part->IsA("Part")) {
            Part* partObj = static_cast<Part*>(part);
            PartType shapeType = StringToPartType(partObj->Shape);
            model = GetPrimitiveModel(shapeType);
        } else {
            // Default to block for other BasePart types
            model = GetPrimitiveModel(PartType::Block);
        }

        if (!model) continue;

        // Set up model matrix
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(
            part->Position.x, part->Position.y, part->Position.z
        ));
        
        // Apply rotation (convert from degrees to radians)
        modelMat = glm::rotate(modelMat, glm::radians(part->Rotation.x), glm::vec3(1, 0, 0));
        modelMat = glm::rotate(modelMat, glm::radians(part->Rotation.y), glm::vec3(0, 1, 0));
        modelMat = glm::rotate(modelMat, glm::radians(part->Rotation.z), glm::vec3(0, 0, 1));
        
        // Apply scale
        modelMat = glm::scale(modelMat, glm::vec3(
            part->Size.x, part->Size.y, part->Size.z
        ));

        g_basicShader->SetMat4("model", modelMat);
        g_basicShader->SetVec3("objectColor", glm::vec3(
            part->Color.r, part->Color.g, part->Color.b
        ));
        g_basicShader->SetFloat("transparency", part->Transparency);

        // Draw the model
        model->Draw();
    }

    g_basicShader->Unbind();
}

void PrepareRenderer() {
    GenerateDefaultTexture();
    PreparePrimitiveModels();
    
    // Create basic shader
    g_basicShader = new Engine::Graphics::Shader(basicVertexShader, basicFragmentShader);
}

void UnprepareRenderer() {
    if (g_defaultTexture) {
        delete g_defaultTexture;
        g_defaultTexture = nullptr;
    }
    if (g_basicShader) {
        delete g_basicShader;
        g_basicShader = nullptr;
    }
}

glm::mat4 GetLightSpaceMatrix(const Vector3 &lightDir, const Vector3 &sceneCenter) {
    // Stub for now
    return glm::mat4(1.0f);
}

void RenderShadowMap(const std::vector<BasePart *> &instances,
                     const Vector3 &lightDir, const Vector3 &sceneCenter) {
    // Stub for now
}