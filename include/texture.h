#pragma once

#include <stdint.h>

typedef struct {
    uint32_t id;
    int32_t width;
    int32_t height;
    int32_t channel_count;
} Texture;

Texture* texture_create();

void texture_load_from_file(Texture* texture, const char* const path);

void texture_bind_0(Texture* texture);