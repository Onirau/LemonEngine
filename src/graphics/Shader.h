#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine::Graphics {

    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        // Uniform setters
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

    private:
        unsigned int m_RendererID;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        int GetUniformLocation(const std::string& name);
        unsigned int CompileShader(unsigned int type, const std::string& source);
        unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    };

}
