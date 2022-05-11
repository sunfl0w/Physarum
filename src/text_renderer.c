#include "text_renderer.h"

#include "deps/glad/glad.h"

#include <stdlib.h>
#include <string.h>

TextRenderer* text_renderer_create() {
    TextRenderer* text_renderer = (TextRenderer*)malloc(sizeof(TextRenderer));

    float vertices[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};
    uint32_t indices[] = {0, 1, 3, 1, 2, 3};

    glGenVertexArrays(1, &text_renderer->vao);
    glGenBuffers(1, &text_renderer->vbo);
    glGenBuffers(1, &text_renderer->ebo);

    glBindVertexArray(text_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, text_renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text_renderer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return text_renderer;
}

void text_renderer_free(TextRenderer* text_renderer) {
    if (text_renderer) {
        free(text_renderer);
    }
}

void text_renderer_draw_text(TextRenderer* text_renderer, const char* text, Texture* font_texture, ShaderProgram* shader_program, int32_t x, int32_t y, float scale) {
    /*float left = 0.0f;
    float right = SCREEN_WIDTH;
    float bottom = 0.0f;
    float top = SCREEN_HEIGHT;
    float near = 0.1f;
    float far = 1000.0f;*/

    float m[] = {scale, 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, scale * x, scale * y, 1.0f, 1.0f};
    //float vp[] = {2.0f / (right - left),        0.0f, 0.0f, 0.0f, 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f, 0.0f, 0.0f, -2.0f / (far - near), 0.0f, -(right + left) / (right - left), -(top + bottom) / (top - bottom),
    //              -(far + near) / (far - near), 1.0f};

    for (int i = 0; i < strlen(text); i++) {
        uint32_t char_index = 0;
        if (text[i] >= 32 && text[i] < 128) {
            char_index = text[i] - 32;
        }
        float char_x = (char_index % 16) / 16.0f;
        float char_y = (char_index / 16) / 6.0f;

        float texture_pos_left = char_x;
        float texture_pos_right = char_x + 1 / 16.0f;
        float texture_pos_top = 1 - char_y;
        float texture_pos_bottom = 1 - char_y - 1 / 6.0f;

        glBindBuffer(GL_ARRAY_BUFFER, text_renderer->vbo);
        float vertexData[] = {1.0f, 1.0f, texture_pos_right, texture_pos_top,    1.0f, 0.0f, texture_pos_right, texture_pos_bottom,
                              0.0f, 0.0f, texture_pos_left,  texture_pos_bottom, 0.0f, 1.0f, texture_pos_left,  texture_pos_top};
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        shader_program_use(shader_program);
        shader_program_set_uniform_int(shader_program, "tex", 0);
        shader_program_set_uniform_mat4(shader_program, "m", m);

        texture_bind_0(font_texture);
        glBindVertexArray(text_renderer->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        float trans[] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
        float m_new[16];
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                float sum = 0.0f;
                for (int z = 0; z < 4; z++) {
                    sum += m[y + 4 * z] * trans[z + 4 * x];
                }
                m_new[y + x * 4] = sum;
            }
        }
        for(int i = 0; i < 16; i++) {
            m[i] = m_new[i];
        }
    }
}