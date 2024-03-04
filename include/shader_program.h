#pragma once

#include "shader.h"

#include <stdint.h>

#define SHADER_PROGRAM_LINK_MAX_LOG_SIZE 1024
#define SHADER_PROGRAM_LINK_SUCCESS 0
#define SHADER_PROGRAM_LINK_ERROR 1

typedef struct {
    uint32_t id;
} ShaderProgram;

ShaderProgram* shader_program_create();

int shader_program_link(ShaderProgram* shader_program, Shader* vertex_shader, Shader* fragment_shader);

int shader_program_link_compute(ShaderProgram* shader_program, Shader* compute_shader);

void shader_program_use(ShaderProgram* shader_program);

void shader_program_set_uniform_int(ShaderProgram* shader_program, const char* const uniform_name, uint64_t value);

void shader_program_set_uniform_float(ShaderProgram* shader_program, const char* const uniform_name, float value);

void shader_program_set_uniform_vec4(ShaderProgram* shader_program, const char* const uniform_name, float* value);

void shader_program_set_uniform_mat4(ShaderProgram* shader_program, const char* const uniform_name, float* value);