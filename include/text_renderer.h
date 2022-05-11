#pragma once
#include <stdint.h>
#include "shader_program.h"
#include "texture.h"

typedef struct {
    uint32_t vao, vbo, ebo;
} TextRenderer;

TextRenderer* create_text_renderer();

free_text_renderer(TextRenderer* text_renderer);

void draw_text(TextRenderer* text_renderer, const uint8_t* text, Texture* font_texture, ShaderProgram* shader_program, uint32_t x,  uint32_t y,  uint32_t scale);