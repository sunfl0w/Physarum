#include "shader.hpp"

namespace Tracer::Rendering {
    Shader::Shader() {}

    Shader::Shader(std::string shaderPath, GLenum shaderType) {
        unsigned int shader = CompileShaderFromFile(shaderPath, shaderType);
        LinkShaderProgram(std::vector<unsigned int> {shader});
    }

    Shader::Shader(std::string shaderPath0, GLenum shaderType0, std::string shaderPath1, GLenum shaderType1) {
        unsigned int shader0 = CompileShaderFromFile(shaderPath0, shaderType0);
        unsigned int shader1 = CompileShaderFromFile(shaderPath1, shaderType1);
        LinkShaderProgram(std::vector<unsigned int> {shader0, shader1});
    }

    void Shader::Activate() const {
        glUseProgram(ID);
    }

    unsigned int Shader::GetID() {
        return ID;
    }

    void Shader::SetInt(std::string name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::SetVec2(std::string name, glm::vec2 vector) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vector));
    }

    void Shader::SetVec4(std::string name, glm::vec4 vector) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vector));
    }

    void Shader::SetMat4(std::string name, glm::mat4 matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    //Private

    unsigned int Shader::CompileShaderFromFile(std::string shaderPath, GLenum shaderType) {
        std::ifstream shaderFileStream;
        std::stringstream shaderStringStream;

        shaderFileStream.open(shaderPath);
        shaderStringStream << shaderFileStream.rdbuf();
        shaderFileStream.close();

        //Compiling shader
        std::string shaderCode = shaderStringStream.str();
        const char* shaderCodePtr = shaderCode.c_str();
        unsigned int shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, 1, &shaderCodePtr, NULL);
        glCompileShader(shaderID);
        //Check for vertex shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
            std::cout << "Shader compilation failed. Log:\n"
                      << infoLog << std::endl;
        }
        return shaderID;
    }

    void Shader::LinkShaderProgram(std::vector<unsigned int> shaderIDs) {
        //Link shaders
        ID = glCreateProgram();
        for(unsigned int shaderID : shaderIDs) {
            glAttachShader(ID, shaderID);
        }
        glLinkProgram(ID);
        //Check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "Shader linking failed. Log:\n"
                      << infoLog << std::endl;
        }
        for(unsigned int shaderID : shaderIDs) {
            glDeleteShader(shaderID);
        }
    }
}  // namespace Tracer::Rendering