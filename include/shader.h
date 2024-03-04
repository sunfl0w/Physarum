#pragma once

#include <stdint.h>

#define SHADER_MAX_LOG_SIZE 1024
#define SHADER_LOAD_SUCCESS 0
#define SHADER_LOAD_INVALID_PATH 1
#define SHADER_LOAD_READ_ERROR 2
#define SHADER_LOAD_COMPILE_ERROR 3

typedef struct {
    uint32_t type;
    uint32_t id;
} Shader;

Shader* shader_create();

int shader_load_from_file(Shader* const shader, const char* const path, uint32_t shader_type);