#pragma once
#include <stdint.h>
#include "shader_program.h"
#include "texture.h"

typedef struct {
    uint32_t vao, vbo, ebo;
} TextRenderer;

TextRenderer* text_renderer_create();

void text_renderer_free(TextRenderer* text_renderer);

void text_renderer_draw_text(TextRenderer* text_renderer, const char* text, Texture* font_texture, ShaderProgram* shader_program, int32_t x,  int32_t y, float scale);