#include "texture.h"

#include "deps/glad/glad.h"
#include "deps/stb/stb_image.h"

Texture* texture_create() {
    return (Texture*)malloc(sizeof(Texture));
}

void texture_load_from_file(Texture* texture, const char* const path) {
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(1);

    unsigned char* image_buffer = stbi_load(path, &texture->width, &texture->height, &texture->channel_count, 0);
    if (image_buffer) {
        if (texture->channel_count == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_buffer);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else if (texture->channel_count == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    stbi_image_free(image_buffer);
}

void texture_bind_0(Texture* texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}