#include "shader.h"

#include "deps/glad/glad.h"

#include <stdio.h>
#include <stdlib.h>

Shader* shader_create() {
    return (Shader*)malloc(sizeof(Shader));
}

int shader_load_from_file(Shader* const shader, const char* const path, uint32_t shader_type) {
    FILE* file = fopen(path, "r");
    char* file_buffer;
    if (file) {
        fseek(file, 0, SEEK_END);
        uint64_t file_size = ftell(file);
        rewind(file);

        file_buffer = (char*)malloc((file_size + 1) * sizeof(char));
        uint64_t read_size = fread(file_buffer, sizeof(char), file_size, file);
        file_buffer[file_size] = '\0';

        if (read_size != file_size) {
            free(file_buffer);
            fclose(file);
            return SHADER_LOAD_READ_ERROR;
        }
        fclose(file);
    } else {
        return SHADER_LOAD_INVALID_PATH;
    }

    const char* shader_buffer = file_buffer;
    shader->id = glCreateShader(shader_type);
    glShaderSource(shader->id, 1, &shader_buffer, NULL);
    glCompileShader(shader->id);
    free(file_buffer);

    int32_t success;
    glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[SHADER_MAX_LOG_SIZE];
        glGetShaderInfoLog(shader->id, SHADER_MAX_LOG_SIZE, NULL, log);
        printf("Shader compiled with errors: \n%s", log);
        return SHADER_LOAD_COMPILE_ERROR;
    }
    return SHADER_LOAD_SUCCESS;
}