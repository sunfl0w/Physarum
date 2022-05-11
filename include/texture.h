#pragma once

#include <stdint.h>

typedef struct {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t channel_count;
} Texture;

Texture* create_texture();

void texture_load_from_file(Texture* texture, const char* const path);

void texture_bind_0(Texture* texture);