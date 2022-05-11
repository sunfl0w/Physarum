#include "shader_program.h"

#include "deps/glad/glad.h"

#include <stdlib.h>

ShaderProgram* shader_program_create() {
    return (ShaderProgram*)malloc(sizeof(ShaderProgram));
}

void shader_program_link(ShaderProgram* shader_program, Shader* vertex_shader, Shader* fragment_shader) {
    shader_program->id = glCreateProgram();
    glAttachShader(shader_program->id, vertex_shader->id);
    glAttachShader(shader_program->id, fragment_shader->id);
    glLinkProgram(shader_program->id);

    int success;
    glGetProgramiv(shader_program->id, GL_LINK_STATUS, &success);
    if (!success) {
        uint8_t log[SHADER_MAX_LOG_SIZE];
        glGetProgramInfoLog(shader_program->id, SHADER_PROGRAM_LINK_MAX_LOG_SIZE, NULL, log);
        printf("Shader program linking error: \n%s", log);
        return SHADER_PROGRAM_LINK_ERROR;
    }
    return SHADER_PROGRAM_LINK_SUCCESS;
}

void shader_program_link_compute(ShaderProgram* shader_program, Shader* compute_shader) {
    shader_program->id = glCreateProgram();
    glAttachShader(shader_program->id, compute_shader->id);
    glLinkProgram(shader_program->id);

    int success;
    glGetProgramiv(shader_program->id, GL_LINK_STATUS, &success);
    if (!success) {
        uint8_t log[SHADER_MAX_LOG_SIZE];
        glGetProgramInfoLog(shader_program->id, SHADER_PROGRAM_LINK_MAX_LOG_SIZE, NULL, log);
        printf("Shader program linking error: \n%s", log);
        return SHADER_PROGRAM_LINK_ERROR;
    }
    return SHADER_PROGRAM_LINK_SUCCESS;
}

void shader_program_use(ShaderProgram* shader_program) {
    glUseProgram(shader_program->id);
}

void shader_program_set_uniform_int(ShaderProgram* shader_program, const char* const uniform_name, uint64_t value) {
    glUniform1i(glGetUniformLocation(shader_program->id, uniform_name), value);
}

void shader_program_set_uniform_vec4(ShaderProgram* shader_program, const char* const uniform_name, float* value) {
    glUniform4fv(glGetUniformLocation(shader_program->id, uniform_name), 1, value);
}

void shader_program_set_uniform_mat4(ShaderProgram* shader_program, const char* const uniform_name, float* value) {
    glUniformMatrix4fv(glGetUniformLocation(shader_program->id, uniform_name), 1, GL_FALSE, value);
}