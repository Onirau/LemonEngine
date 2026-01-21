#include "Renderer.h"

#include "src/graphics/Shader.h"
#include "src/graphics/Texture2D.h"
#include "src/graphics/Model.h"
#include "PrimitiveModels.h"
#include "SkyboxRenderer.h"
#include "../instances/BasePart.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <cmath>

// Define missing raylib-like functions
#define DEG2RAD (3.14159265358979323846f / 180.0f)

// Stub implementations for raylib-like functions
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

Engine::Graphics::Texture2D LoadTextureFromImage(Image img) {
    return Engine::Graphics::Texture2D(img.width, img.height, (unsigned char*)img.data);
}

void UnloadImage(Image img) {
    if (img.data) free(img.data);
}

// Matrix helper functions
glm::mat4 MatrixPerspective(float fovy, float aspect, float nearPlane, float farPlane) {
    return glm::perspective(fovy, aspect, nearPlane, farPlane);
}

glm::mat4 MatrixOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
}

glm::mat4 MatrixLookAt(Vector3 eye, Vector3 target, Vector3 up) {
    glm::vec3 gEye(eye.x, eye.y, eye.z);
    glm::vec3 gTarget(target.x, target.y, target.z);
    glm::vec3 gUp(up.x, up.y, up.z);
    return glm::lookAt(gEye, gTarget, gUp);
}

glm::mat4 MatrixIdentity() {
    return glm::mat4(1.0f);
}

glm::mat4 MatrixMultiply(glm::mat4 left, glm::mat4 right) {
    return left * right;
}

Vector3 Vector3Scale(Vector3 v, float scalar) {
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

Vector3 Vector3Add(Vector3 v1, Vector3 v2) {
    return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3 Vector3Negate(Vector3 v) {
    return Vector3(-v.x, -v.y, -v.z);
}

Vector3 Vector3Normalize(Vector3 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 0.00001f) {
        return Vector3(v.x / length, v.y / length, v.z / length);
    }
    return Vector3(0, 0, 0);
}

#define SHADOW_MAP_SIZE 2048

unsigned int depthFBO = 0;
unsigned int depthTex = 0;
Engine::Graphics::Shader* shadowShader = nullptr;
int lightSpaceMatrixLoc = -1;
int shadowMapLoc = -1;
int lightDirLoc = -1;

Engine::Graphics::Texture2D* g_defaultTexture = nullptr;

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
    g_defaultTexture = new Engine::Graphics::Texture2D(img.width, img.height, (unsigned char*)img.data);
    UnloadImage(img);
}

glm::mat4 GetLightSpaceMatrix(const Vector3 &lightDir, const Vector3 &sceneCenter) {
    Vector3 lightPos = {sceneCenter.x - lightDir.x * 100.0f,
                        sceneCenter.y - lightDir.y * 100.0f,
                        sceneCenter.z - lightDir.z * 100.0f};

    glm::mat4 view = MatrixLookAt(lightPos, sceneCenter, Vector3{0.0f, 1.0f, 0.0f});
    glm::mat4 projection = MatrixOrtho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 200.0f);

    return MatrixMultiply(view, projection);
}

void RenderShadowMap(const std::vector<BasePart *> &instances,
                     const Vector3 &lightDir, const Vector3 &sceneCenter) {
    (void)instances;
    (void)lightDir;
    (void)sceneCenter;
    // Stub implementation
}

void RenderScene(Camera3D camera, const std::vector<BasePart *> instances) {
    (void)camera;
    (void)instances;
    // Stub implementation - draw basic scene
}

void PrepareRenderer() {
    GenerateDefaultTexture();
    PreparePrimitiveModels();
}

void UnprepareRenderer() {
    if (g_defaultTexture) {
        delete g_defaultTexture;
        g_defaultTexture = nullptr;
    }
    if (shadowShader) {
        delete shadowShader;
        shadowShader = nullptr;
    }
}