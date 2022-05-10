#pragma once

#include <glad.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Tracer::Rendering {
    class Shader {
        unsigned int ID;

    public:
        Shader();

        Shader(std::string shaderPath, GLenum shaderType);

        Shader(std::string shaderPath0, GLenum shaderType0, std::string shaderPath1, GLenum shaderType1);

        void Activate() const;

        unsigned int GetID();

        void SetInt(std::string name, int value) const;

        void SetVec2(std::string name, glm::vec2 vector) const;

        void SetVec4(std::string name, glm::vec4 vector) const;

        void SetMat4(std::string name, glm::mat4 matrix) const;

    private:
        unsigned int CompileShaderFromFile(std::string shaderPath, GLenum shaderType);

        void LinkShaderProgram(std::vector<unsigned int> shaderIDs);
    };
}  // namespace Tracer::Rendering